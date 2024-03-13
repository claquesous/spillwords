/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include "qdatetime.h"			// seed for rand()
#include "qapplication.h"
#include "aboutform.h"
#include "qmessagebox.h"
#include "SpillBoard.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "SpillDie.h"
#include "spellcheckform.h"
#include "images/wood.xpm"
#include "images/spillwords.xpm"
#include <stdlib.h>

#define SERVER_GAME_SOCKET 	35431
#define CLIENT_GAME_SOCKET 	35432

void mainForm::init()
{
    setPaletteBackgroundPixmap(wood);
    setIcon(spillwords);
    
    SpillDie::setPlaying(false);
    m_game=new SpillBoard;
    
    m_playing=false;
    m_tournament=false;
    m_netgame=false;
    m_clientfinished=false;
    
    widgetStack->addWidget(m_game);
    widgetStack->raiseWidget(blankPage);
    
    QTime t = QTime::currentTime();		// set random seed
    srand( t.hour()*12+t.minute()*60+t.second()*60 );
  
    timerWidget= new QWidget(this);
    timerWidget->setGeometry(10,40,50,25);
    timerWidget->setPaletteBackgroundColor(black);    
    timerText(QString("%1").arg(m_doc.timeOut())+":00");
}

void mainForm::destroy() 
{
    if (m_netgame && !m_server)
	m_doc.SwapPlayerNames();
    
    if (m_tournament)
    {
	onTournament();
    }
    else if (m_netgame)
    {
	widgetStack->removeWidget(m_netGame);
	delete m_netGame;
    }
    widgetStack->removeWidget(m_game);    
    delete m_game;
}

void mainForm::timerText(QString time)
{
    QBitmap bm(timerWidget->size(),true);
    QPainter paint(&bm,true);
    paint.setFont(QFont("Times New Roman",18));
    paint.drawText(0,0,timerWidget->width(),timerWidget->height(),Qt::AlignRight,time);
    timerWidget->setMask(bm);
}

void mainForm::fileExit()
{
    QApplication::exit(0);
}

void mainForm::helpIndex()
{

}

void mainForm::helpContents()
{

}

void mainForm::helpAbout()
{
    aboutForm w;
    w.exec();
}

void mainForm::onSpill()
{
    // Game is in session means Q without U
    // Tell the net player I've respilled.
    if (m_netgame && m_playing)
	m_game->Send("Q");
    
    if (m_netgame)
    {
	if (m_server || !m_doc.isSame())
	    m_game->NetBeginGame();
	else
	    m_game->CopyDice(m_netGame);
	if (m_server && m_doc.isSame())
	    m_netGame->CopyDice(m_game);
    }    
    else if (m_tournament)
    {// If tournament prompt to play
	m_game=&m_games[m_playernum];
	QString message="It is "+m_doc.getPlayerName(m_playernum)+"'s Turn";
	QMessageBox mb("Turn",message,QMessageBox::NoIcon,QMessageBox::Ok,QMessageBox::Cancel,QMessageBox::NoButton);
	if (!(mb.exec()==QMessageBox::Ok))
	    return;

	if (!m_doc.isSame() || m_playernum==0)
	    m_game->BeginGame();
	if (m_doc.isSame() && m_playernum==0)
	{ // First player copies dice to other games	    
	    for (int i=1; i<m_doc.getNumPlayers(); i++)
		m_games[i].CopyDice(m_game);
	}	
    }
    else
	m_game->BeginGame();
    
    statusBar()->clear();    
    m_playing=true;
    // Geoff Crew can always run Best (for debugging purposes of course)
    bestAction->setEnabled(m_doc.GetCurrentPlayerName()=="Geoff Crew");
    spillAction->setEnabled(m_game->QNoU());
    endAction->setEnabled(true);
    optionsAction->setEnabled(false);    
    
    widgetStack->raiseWidget(m_game);
    SpillDie::setPlaying(m_playing);    
    
    // Set timer
    m_time.start();
    game_timer=startTimer(250);
    
    if (m_tournament && m_doc.ComputersTurn(m_playernum))
    {// AI plays a game
	m_game->bestPuzzle(m_doc.getAI(),m_doc.getAcc());
	onEnd();
    }
}

void mainForm::onBest()
{
    if (QMessageBox::warning(this,"Warning","Calculating could take a while.  Proceed?","OK","Cancel")==0)
	m_game->bestPuzzle(225,25);
}

void mainForm::onTournament()
{
    // Change tournament mode when clicked on
    m_tournament=!m_tournament;
    namesAction->setEnabled(!m_tournament);
    scoresAction->setEnabled(m_tournament);
    
    if (m_tournament)
    {
	widgetStack->removeWidget(m_game);
	delete m_game;
	m_games = new SpillBoard[m_doc.getNumPlayers()];
	for (int i=0; i<m_doc.getNumPlayers(); i++)
	    widgetStack->addWidget(&m_games[i]);
	m_playernum=0;
    }
    else 
    {
	for (int i=0; i<m_doc.getNumPlayers(); i++)
	    widgetStack->removeWidget(&m_games[i]);
	delete [] m_games;
	m_game=new SpillBoard;
	widgetStack->addWidget(m_game);
    }
}

void mainForm::onEnd()
{
    killTimer(game_timer);
    m_playing=false;
    SpillDie::setPlaying(m_playing);
    m_playernum++;
    
    if (m_netgame)
    {// Tells opponent we're done
	m_game->NetEndGame();
	// If Simultaneous, also score his game
	if (!m_doc.isNetWatch() && m_clientfinished)
	    m_netGame->NetSendScore(m_netscore=m_netGame->ScoreGame(m_doc.getPlayerName(0)));	
    }
    else if (m_tournament && m_doc.isSame() && m_playernum<m_doc.getNumPlayers())
    {// In Same Mode after first player so let subsequent players go without scoring.	
	// Clears the screen
	widgetStack->raiseWidget(blankPage);
    }
    else
    {
	// Last player just finished a same game
	if (m_tournament && m_doc.isSame() && m_playernum==m_doc.getNumPlayers())
	{
	    for (int i=0; i<m_playernum; i++)
	    {
		m_game=&m_games[i];
		int score=m_game->ScoreGame(m_doc.getPlayerName((i+1)%m_playernum));
		QString mesg=m_doc.getPlayerName(i)+"'s Score for this Round: " + QString("%1").arg(score);
		QMessageBox::information(this,"Score",mesg,"OK");
		m_doc.AddScore(score);
	    }
	    m_viewing=m_playernum-1;
	    m_playernum=0;
	}
	else
	{
	    int score;	    
	    if (m_tournament)
		score=m_game->ScoreGame(m_doc.getPlayerName((m_playernum+1)%m_doc.getNumPlayers()));
	    else
		score=m_game->ScoreGame(m_doc.getPlayerName(0));
    
	    QString mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(score);

	    QMessageBox::information(this,"Score",mesg,"OK");
	    
	    if (m_tournament)
	    {
		m_doc.AddScore(score);
		if (m_playernum==m_doc.getNumPlayers())
		{
		    m_viewing=m_playernum-1;
		    m_playernum=0;
		}
		m_game=&m_games[m_playernum];
	    }
	}
	bestAction->setEnabled(true);	
    }
    endAction->setEnabled(false);
    if (!m_netgame)
	spillAction->setEnabled(true);
    optionsAction->setEnabled(true);
}

void mainForm::onMessage()
{
    messageForm w;
    
    if (w.exec()==QDialog::Accepted)
    {
	if (m_server)
	    m_game->Send("M"+ m_doc.getPlayerName(0) + ": "+w.getMessage());
	else
	    m_game->Send("M"+ m_doc.getPlayerName(1) + ": "+w.getMessage());
    }
}

void mainForm::onNet()
{
    // Start a net game
    netForm netdlg;
    if (netdlg.exec()==QDialog::Accepted)
    {
	m_netGame = new SpillBoard;
	widgetStack->addWidget(m_netGame);
	if (m_server=netdlg.isServer())
	{	
	    // Game listens and netGame listens for simultaneous
	    m_game->StartServer(SERVER_GAME_SOCKET);
	    m_netGame->StartServer(CLIENT_GAME_SOCKET);
	}
	else
	{
	    m_game->StartClient(netdlg.getAddress(),CLIENT_GAME_SOCKET);
	    m_netGame->StartClient(netdlg.getAddress(),SERVER_GAME_SOCKET);
	}
	m_netgame=true;
	disconnectAction->setEnabled(true);
	netAction->setEnabled(false);
	spillAction->setEnabled(false);
	tournamentAction->setEnabled(false);
	namesAction->setEnabled(false);
	scoresAction->setEnabled(true);
	m_doc.resetScores();
	m_doc.resetTime();
    }
}

bool mainForm::event( QEvent * e ) 
{
    if (m_tournament && !m_playing && m_playernum==0 && m_doc.endOfRound())
    {
	if ( e->type() == QEvent::KeyPress ) 
	{
	    QKeyEvent * ke = (QKeyEvent*) e;
	    if (ke->key()==Qt::Key_Right || ke->key()==Qt::Key_Up)
	    {
		m_viewing++;
		m_viewing=m_viewing%m_doc.getNumPlayers();
	    }
	    else if (ke->key()==Qt::Key_Left || ke->key()==Qt::Key_Down)
	    {
		m_viewing--;
		if (m_viewing<0)
		    m_viewing=m_doc.getNumPlayers()-1;
	    }
	    else
		return QWidget::event(e);
	    widgetStack->raiseWidget(&m_games[m_viewing]);	    
	    statusBar()->message("Nowing showing " +m_doc.getPlayerName(m_viewing) +"'s Game.");
	    return TRUE;
	}
    }
    return QWidget::event( e );
}

void mainForm::closeEvent( QCloseEvent * )
{
    fileExit();
}

void mainForm::timerEvent(QTimerEvent *e) 
{
    if (e->timerId()==game_timer)
    {// Timer for local timer and game
	char timer[5];
	int secsLeft=m_doc.timeOut()*60-m_time.elapsed()/1000;
	sprintf(timer,"%d:%.2d",secsLeft/60,secsLeft%60);
	timerText(timer);
	
	// Turn off respill for Q no U after 30 seconds
	if (secsLeft==m_doc.timeOut()*60-30)
	    spillAction->setEnabled(false);
	else if (secsLeft<0)
	{
	    timerText("0:00");
	    if (m_playing)
		onEnd();
	    else // Networked player just watching time
		killTimer(game_timer);
	}
	if (m_netgame && m_playing)
	    m_game->NetSendDice();
    }
}

void mainForm::onDisconnect()
{
    if (QMessageBox::warning(this,"Disconnect","Do you really want to disconnect?","&Yes","&No",QString::null,1)==0)
    {
	m_server=false;
	m_netgame=false;
	m_game->Close();
	if (m_netGame->Connected())
	    m_netGame->Close();
	
	disconnectAction->setEnabled(false);
	messageAction->setEnabled(false);
	netAction->setEnabled(true);
	
	spillAction->setEnabled(true);
	tournamentAction->setEnabled(true);
	namesAction->setEnabled(true);
	scoresAction->setEnabled(false);
	m_doc.resetScores();
    }
}

void mainForm::onOptions()
{
    m_doc.UpdateOptions();
    timerText(QString("%1").arg(m_doc.timeOut())+":00");
}

void mainForm::onNames()
{
    m_doc.UpdateNames();
}

void mainForm::onScores()
{
    if ( m_doc.DisplayScores() )
	m_playernum=0;
}

void mainForm::onViewTimer()
{
    if (timerWidget->isVisible())
	timerWidget->hide();
    else
	timerWidget->show();
}

void mainForm::onViewStatus()
{
    if (statusBar()->isVisible())
	statusBar()->hide();
    else
	statusBar()->show();
}

/////////////////////////////////////////////////////////////////////////////////////
// Functions that are called in response to Net messages

void mainForm::serverConnected()
{
    if (m_game->Connected() && m_netGame->Connected())
    {
	statusBar()->message("A server connection has been established.",2000);
	m_game->Send("C"+m_doc.getPlayerName(0));
	if (m_doc.isNetWatch())
	    m_netGame->Send("OW");
	else if (m_doc.isSame())
	    m_netGame->Send("OS");
	else
	    m_netGame->Send("OD");
	messageAction->setEnabled(true);
	spillAction->setEnabled(true);
    }   
}

void mainForm::clientConnected(char mode)
{
    if (m_netGame->Connected())
	m_netGame->Send("C"+m_doc.GetCurrentPlayerName());

    statusBar()->message("A client connection has been established.",2000);
	
    m_doc.setSame(mode=='S');
    m_doc.setNetWatch(mode=='W');

    messageAction->setEnabled(true);
}

void mainForm::receivedFaces()
{
    if (m_doc.isNetWatch())
    {
	m_playing=false;
	widgetStack->raiseWidget(m_netGame);
	m_time=QTime::currentTime();
	game_timer=startTimer(250);
    }
    else if (!m_server)
	onSpill();
}

void mainForm::lostConnection()
{
    if (m_netgame)
    {
	m_netgame=false;
	QMessageBox::critical(this,"Connection Lost","Your connection has been lost!");
	
	m_server=false;
	
	disconnectAction->setEnabled(false);
	messageAction->setEnabled(false);
	netAction->setEnabled(true);
	
	spillAction->setEnabled(true);
	tournamentAction->setEnabled(true);
	namesAction->setEnabled(true);
	scoresAction->setEnabled(false);
	m_doc.resetScores();
    }
}

void mainForm::netQNoU()
{
    // Reset timer
    m_time=QTime::currentTime();
    game_timer=startTimer(250);
    statusBar()->message("Net player respilled for Q No U",2000);
}

void mainForm::setNetPlayer(QString player)
{
    m_doc.SetPlayerName(player,1);
    if (!m_server)
	m_doc.SwapPlayerNames();
}

void mainForm::gotNetScore(int netscore)
{
    if (m_doc.isNetWatch())
    {
	QString mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(netscore);
	QMessageBox::information(this,"Score",mesg,"OK");
	m_doc.AddScore(netscore);
	spillAction->setEnabled(false);
    }
    else
    {
	QString mesg;
	if(m_server)
	{
	    mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(netscore);
	    QMessageBox::information(this,"Score",mesg,"OK");
	    m_doc.AddScore(netscore);
	    mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(m_netscore);
	    QMessageBox::information(this,"Score",mesg,"OK");
	    m_doc.AddScore(m_netscore);
	}		
	else
	{
	    m_netGame->NetSendScore(m_netscore=m_netGame->ScoreGame(m_doc.getPlayerName(1)));
	    mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(m_netscore);
	    QMessageBox::information(this,"Score",mesg,"OK");
	    m_doc.AddScore(m_netscore);
	    mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(netscore);
	    QMessageBox::information(this,"Score",mesg,"OK");
	    m_doc.AddScore(netscore);
	}
	
	spillAction->setEnabled(m_server);
	m_clientfinished=false;
    }
}

void mainForm::netFinish()
{
    if (m_doc.isNetWatch())
    {
	killTimer(game_timer);
	int score=m_netGame->ScoreGame("");
	m_netGame->NetSendScore(score);	
	
	QString mesg=m_doc.GetCurrentPlayerName()+"'s Score for this Round: " + QString("%1").arg(score);
	QMessageBox::information(this,"Score",mesg,"OK");
	m_doc.AddScore(score);
	
	spillAction->setEnabled(true);
    }  
    else if (m_server)
    {
	m_clientfinished=true;
	if (!m_playing)
	    m_netGame->NetSendScore(m_netscore=m_netGame->ScoreGame(""));
    }
}

void mainForm::onCheck()
{
    spellCheckForm w;
    w.exec();
}
