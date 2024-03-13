// SpillGame.cpp: implementation of the SpillBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "SpillBoard.h"
#include "SpillDie.h"
#include "SpillAI.h"
#include "spellform.h"
#include "SpillSplash.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qstatusbar.h"
#include "SpillDie.h"
#include "mainform.h"
#include <unistd.h>	// For sleep
#include "qsocket.h"
#include "qserversocket.h"
#include "images/net.xpm"
#include <stdlib.h>
#include "Spillwords.h"
#include "SpillDictionary.h"

class SpillServerSocket : public QServerSocket
{
public:
    SpillServerSocket(int port, SpillBoard *parent) : QServerSocket(port,1,(QObject*)parent) { m_pGame=parent; }
private:
    void newConnection(int socket) { m_pGame->Accept(socket); }
    SpillBoard *m_pGame;
};

SpillBoard::SpillBoard() : QWidget(getMainWindow())
{
    m_socket=NULL;
    m_tempserve=NULL;

    //Set up Dice
    Dice[0] = new SpillDie(this,"TRNPAU");
    Dice[1] = new SpillDie(this,"CAWBSY"); 
    Dice[2] = new SpillDie(this,"BAEXUW"); 
    Dice[3] = new SpillDie(this,"BFHAED"); 
    Dice[4] = new SpillDie(this,"ALGYEV"); 
    Dice[5] = new SpillDie(this,"EQAIDS"); 
    Dice[6] = new SpillDie(this,"MOLPVI"); 
    Dice[7] = new SpillDie(this,"KLGCIJ");
    Dice[8] = new SpillDie(this,"YAEZTD");
    Dice[9] = new SpillDie(this,"MSOUTR");
    Dice[10]= new SpillDie(this,"GIECTD");
    Dice[11]= new SpillDie(this,"OEFTIN");
    Dice[12]= new SpillDie(this,"ONRIHE");
    Dice[13]= new SpillDie(this,"LEIKMN");
    Dice[14]= new SpillDie(this,"USONRH");
}

SpillBoard::~SpillBoard()
{
   for (int i=0; i<15; i++)
	delete Dice[i];

    // Close network connection
    if (Connected())
	Close();
}

void SpillBoard::BeginGame() 
{
    // Roll the dice
    for (int i=0; i<15; i++)
	Dice[i]->roll(true);
}

int SpillBoard::ScoreGame(QString scorer)
{
    QString word;
    for (int i=0; i<15; i++)
    {// Does dice start a word or is it unused?
	word=Dice[i]->getRightWord();
	if (word.length()>0)
	{
	    // Prompts user to decide if word is real
	    if (!(spillDictionary.checkWord(word) || UserSpellCheck(word,scorer)))
	    {
		Dice[i]->detachRightWord();
		// When a net game, needs to send spell check result to opponent
		if (Connected())
		    Send("S" + word);
	    }
	}
	word=Dice[i]->getDownWord();
	if (word.length()>0)
	{//Up to Down word
	    if (!(spillDictionary.checkWord(word) || UserSpellCheck(word,scorer)))
	    {
		Dice[i]->detachDownWord();
		if (Connected())
		    Send("S" + word);
	    }
	}	
    }

    int score=-225;
    for (int i=0; i<15; i++)
    {
	int blockScore=Dice[i]->getBlockScore();
	if (blockScore>score)
	    score=blockScore;
    }
    return score;    
}

void SpillBoard::NetEndGame()
{
    NetSendDice();
    for (int i=0; i<15; i++)
    {
	if (Dice[i]->getRightWord()!="" || Dice[i]->getDownWord()!="")
	    Send("A" + QString("%1").arg(i));
    }
    Send("E");
}

void SpillBoard::resizeEvent(QResizeEvent *re) 
{    
    int x = width();
    int y = height();
    int oldx = re->oldSize().width();
    int oldy = re->oldSize().height();
    int dicewidth;
    
    // Sets the size of dice so they can be resized on screen.
    if (y<x)
	dicewidth=y/15;
    else
	dicewidth=x/15;
    
    if (dicewidth<25)
	dicewidth=25;
    
    for (int i=0; i<15; i++)
    {
	Dice[i]->SetSize(dicewidth);
	Dice[i]->SetPosition(QPoint((Dice[i]->pos().x()*x)/oldx,(Dice[i]->pos().y()*y)/oldy));
    }
}

bool SpillBoard::QNoU()
{// Finds if there is a Q in the puzzle but no U
    bool qfound=false,ufound=false;
    for (int i=0; i<15; i++)
    {
	if (Dice[i]->GetFace()=='Q')
	    qfound=true;
	else if (Dice[i]->GetFace()=='U')
	    ufound=true;
    }
    
    return (qfound && !ufound);
}

void SpillBoard::Receive()
{
    while (m_socket->canReadLine())
    {
	QString pBuf=m_socket->readLine();
	// S Spell Check	
	if (pBuf[0]=='S')
	    getMainWindow()->statusBar()->message("Player rejected " +pBuf.mid(1), 2000);
	// Q Net player respilled because of QnoU
	else if (pBuf[0]=='Q')
	    getMainWindow()->netQNoU();
	// N Received Netscore
	else if (pBuf[0]=='N')
	    getMainWindow()->gotNetScore(atoi(pBuf.mid(1)));
	// F Received faces
	else if (pBuf[0]=='F')
	{
	    for (int i=0; i<15; i++)
		Dice[i]->SetFace(QChar(pBuf[i+1]).latin1());
	    getMainWindow()->receivedFaces();
	}
	// C Connected
	else if (pBuf[0]=='C')
	    getMainWindow()->setNetPlayer(pBuf.mid(1,pBuf.length()-2));
	// O Set options mode
	else if (pBuf[0]=='O')
	    getMainWindow()->clientConnected(pBuf[1].latin1());
	// W Size of board
	else if (pBuf[0]=='W')
	    resize(atoi(pBuf.mid(1,6)),atoi(pBuf.mid(7,6)));
	// D Dice positions
	else if (pBuf[0]=='D')
	{
	    for (int i=0; i<15; i++)
		Dice[i]->SetPosition(QPoint(atoi(pBuf.mid(i*8+1,4)),atoi(pBuf.mid(i*8+5,4))));
	}
	// M Message
	else if (pBuf[0]=='M')
	    getMainWindow()->statusBar()->message(pBuf.mid(1));
	// E Player ended
	else if (pBuf[0]=='E')
	    getMainWindow()->netFinish();
	// A Make attachments
	else if (pBuf[0]=='A')
	    Dice[atoi(pBuf.mid(1))]->Attach();
    }
}

void SpillBoard::Send(QString netmsg)
{
    //send netmsg out the socket
    QTextStream os(m_socket);
    os << netmsg << "\n";
}

void SpillBoard::Close()
{
    if (m_tempserve!=NULL)
	delete m_tempserve;
    if (m_socket!=NULL)
    {
	m_socket->close();
	delete m_socket;
    }
    getMainWindow()->lostConnection();
}

void SpillBoard::NetBeginGame()
{
    // Show a little splash screen
    SpillSplash wndSplash(net);   
    
    BeginGame();
  
    // Send positions of dice
    NetSendDice();
  
    // Send faces		
    QString faces="F";
    for (int i=0; i<15; i++)
	faces.append(Dice[i]->GetFace());
    Send(faces);
    
    // Send width of board
    char size[13];
    sprintf(size,"W%.6d%.6d",width(),height());
    Send(size);
    
    sleep(1);
    wndSplash.close();    
}

void SpillBoard::NetSendDice()
{
    char net[15];
    QString netmsg="D";
    for (int i=0; i<15; i++)
    {// Calculates string to send dice position
	sprintf(net,"%.4i%.4i",Dice[i]->pos().x(),Dice[i]->pos().y());
	netmsg.append(net);
    }
    Send(netmsg);
}

void SpillBoard::NetSendScore(int score)
{// Send score
    Send("N" + QString("%1").arg(score));
}

void SpillBoard::CopyDice(SpillBoard *game)
{// Copy dice faces and dice positions from one game to this game
    for (int i=0; i<15; i++)
    {
	Dice[i]->SetFace(game->Dice[i]->GetFace());
	Dice[i]->SetPosition(game->Dice[i]->pos());
    }
}

void SpillBoard::StartServer(int port)
{
    m_tempserve=new SpillServerSocket(port,this);
}

void SpillBoard::StartClient(QString address, int port)
{   
    m_socket = new QSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(Receive()) );
    connect(m_socket, SIGNAL(connectionClosed()), this, SLOT(Close()) );    
    m_socket->connectToHost(address,port);
}

void SpillBoard::Accept(int socket)
{
    m_socket = new QSocket();
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(Receive()) );
    connect(m_socket, SIGNAL(connectionClosed()), this, SLOT(Close()) );
    m_socket->setSocket(socket);
    getMainWindow()->serverConnected();
    delete m_tempserve;
}

bool SpillBoard::Connected(void)
{
    return (m_socket!=NULL && m_socket->state()==QSocket::Connected);
}

void SpillBoard::bestPuzzle(int intelligence, int accuracy)
{
    SpillAI spillAI(Dice, intelligence, accuracy);
    spillAI.getBestPuzzle();
    getMainWindow()->statusBar()->message("AI Finished",1000);
}

bool SpillBoard::UserSpellCheck(QString strWord, QString scorer)
{
    /*
    QStringArray custarray;
    // Load the Custom Dictionary
    CFileStatus status2;
    if(CFile::GetStatus(AfxGetApp()->GetProfileString("Settings", "Custom", ""), status2)) // See if a custom dictionary exists
    {
	CFile cfSettingsFile (AfxGetApp()->GetProfileString("Settings", "Custom", ""), CFile::modeNoTruncate | CFile::modeReadWrite );
	CArchive ar ( &cfSettingsFile, CArchive::load );
	custarray.Serialize ( ar ); // Load the custom dictionary
    }
    */
    spellForm dlg;
    dlg.setText(scorer,strWord);
    
    // Display the Spell Dialog and wait for the user to do something
    if (dlg.exec() == QDialog::Accepted) 
    {
	if (dlg.m_add) // Add Word was clicked, so add the word to the custom dictionary
	{// Add Button Clicked
/*	    QString strAddWord = strWord;
	    strAddWord.MakeLower();
	    
	    custarray.Add(strAddWord);
	    
	    CFile cfSettingsFile (AfxGetApp()->GetProfileString("Settings", "Custom", ""), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite );
	    CArchive ar2 ( &cfSettingsFile, CArchive::store );
	    custarray.Serialize ( ar2 ); // Save the custom dictionary					*/
	}
	return true;
    }
    return false;
}


