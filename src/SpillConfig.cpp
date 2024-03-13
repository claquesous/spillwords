// SpilNSpelDoc.cpp : implementation of the CMySpilNSpelDoc class
//

#include "SpillConfig.h"
#include "scoresform.h"
#include "playersform.h"
#include "optionsform.h"
#include "qmessagebox.h"
#include "qfile.h"
#include "SpillDictionary.h"
#include <stdlib.h>
#include "qradiobutton.h"

SpillConfig::SpillConfig()
{
    finished=false;
    QString optionsFile = QString(getenv("HOME"))+QString("/.spillwords");
    QFile file(optionsFile);
    int boolValue;
    if (file.open( IO_ReadOnly))
    {
	QDataStream stream( &file );
	stream>>m_strMain>>m_aspell;
	stream>>m_timeout>>m_ai>>m_aiacc>>m_endscore;
	stream>>boolValue;
	m_same=boolValue==1;
	stream>>boolValue;
	m_netwatch=boolValue==1;
      stream>>m_dictmode;
	
	stream>>m_numscores;
	for (unsigned int i=0; i<m_numscores; i++)
	{
	    int score;
	    stream>>score;
	    m_scores<<score;
	}
	
	while ( !stream.atEnd())
	{
	    QString player;
	    stream>>player;
	    m_players<<player;
	}
	
	file.close();
    }
    else
    {
	m_timeout=3;
	m_ai=75;
	m_aiacc=15;
	m_endscore=300;
	m_players <<"Player" << "Computer (C)";
	m_same=false;
	m_netwatch=false;
	m_numscores=0;
	m_strMain="dict.dct";
      m_aspell="en_US";
      m_dictmode=DICTIONARY_MODE_FILE;
    }
    spillDictionary.UpdateDictionary(m_strMain);
    spillDictionary.setMode(m_dictmode);
}

SpillConfig::~SpillConfig()
{
    if (finished)
    {
	m_numscores=0;
	m_scores.clear();
	finished=false;
    }    
    QString optionsFile = QString(getenv("HOME"))+QString("/.spillwords");
    QFile file(optionsFile);

    QStringList lines;
    if ( file.open( IO_WriteOnly ) ) 
    {
	QDataStream stream( &file );
	stream<<m_strMain<<m_aspell;
	stream<<m_timeout<<m_ai<<m_aiacc<<m_endscore;
	if (m_same)
	    stream<<1;
	else
	    stream<<0;
	if (m_netwatch)
	    stream<<1;
	else
	    stream<<0;
      stream<<m_dictmode;

	stream<<m_numscores-(m_numscores%m_players.size());
	for (unsigned int i=0; i<m_numscores-(m_numscores%m_players.size()); i++)
	    stream<<m_scores[i];
	
	for ( QStringList::Iterator it = m_players.begin(); it != m_players.end(); ++it )
	    stream << *it;
	file.close();
    }
    else
	printf("Couldn't open "+ optionsFile + "for editing!\n");
}


void SpillConfig::AddScore(int newScore)
{
    // If this is the beginning of a new tournament clear old scores
    if (finished)
    {
	m_numscores=0;
	m_scores.clear();
	finished=false;
    }
    // Add a new score and increment counter, Check for end of game    
    m_scores<<newScore;
    m_numscores++;
    if (m_numscores%m_players.size()==0)
	GameOver();
}

bool SpillConfig::DisplayScores()
{
    // Show scores dialog
    scoresForm scdlg;
    scdlg.m_players=m_players;
    scdlg.m_scores=m_scores;
    scdlg.m_numscores=m_numscores;
    scdlg.exec();
    if (scdlg.m_cleared)
    {
	m_numscores=0;
	m_scores.clear();
    }
    return scdlg.m_cleared;
}

void SpillConfig::resetScores()
{
    m_numscores=0;
    m_scores.clear();
}

bool SpillConfig::GameOver(void)
{
    int winningscore=m_endscore;
    QStringList winningplayers;
    QValueList<int> scores; //int scores[m_players.size()];
    for (unsigned int i=0; i<m_players.size(); i++)
      scores<<0;
    for (unsigned int i=0; i<m_numscores; i++)
    {
	if (i<m_players.size())
	    scores[i]=0;
	scores[i%m_players.size()]+=m_scores[i];
	if (scores[i%m_players.size()]>=winningscore)
	{
	    if (scores[i%m_players.size()]>winningscore)
	    {
		winningscore=scores[i%m_players.size()];
		winningplayers.clear();
	    }
	    winningplayers<<m_players[i%m_players.size()];
	    QString mesg="The following players have achieved a score of: " 
			 + QString("%1").arg(winningscore) +"\n"
			 + winningplayers.join(", ");
	    QMessageBox mb("Game Over",mesg,QMessageBox::NoIcon,QMessageBox::Ok,
			   QMessageBox::NoButton,QMessageBox::NoButton);
	    mb.exec();
	    
	    //Show final scores
	    DisplayScores();
	    finished=true;
	    return true;
	}
    }
    return false;
}

QString SpillConfig::GetCurrentPlayerName(void)
{
    if (m_players[m_numscores%m_players.size()].endsWith(" (C)"))
	return m_players[m_numscores%m_players.size()].left(m_players[m_numscores%m_players.size()].length()-4);
    return m_players[m_numscores%m_players.size()];
}

bool SpillConfig::endOfRound(void)
{
    return m_numscores%m_players.size()==0 && m_numscores>0;
}

void SpillConfig::SetPlayerName(QString newName, int player)
{
    m_players[player]=newName;
}

void SpillConfig::SwapPlayerNames(void)
{
    QString temp=m_players[0];
    m_players[0]=m_players[1];
    m_players[1]=temp;
}

bool SpillConfig::ComputersTurn(int player)
{
    return m_players[player%m_players.size()].endsWith(" (C)");
}

void SpillConfig::UpdateOptions()
{
      // Show options and store information
    optionsForm optdlg;

    optdlg.m_ai=m_ai;
    optdlg.m_aiacc=m_aiacc;
    optdlg.m_endscore=m_endscore;
    optdlg.m_same=m_same;
    optdlg.m_watch=m_netwatch;
    optdlg.m_strMain=m_strMain;
    optdlg.m_aspell=m_aspell;
    optdlg.m_time=m_timeout;
    if (m_numscores%m_players.size()!=0)
	optdlg.m_nosame=true;
    optdlg.m_dictmode=m_dictmode;
    optdlg.exec();
    if (optdlg.result() == QDialog::Rejected)
      return;
    m_netwatch=optdlg.m_watch;
    m_same=optdlg.m_same;
    m_ai=optdlg.m_ai;
    m_aiacc=optdlg.m_aiacc;
    m_endscore=optdlg.m_endscore;
    if (m_strMain!=optdlg.m_strMain)
    {
      m_strMain=optdlg.m_strMain;
      spillDictionary.UpdateDictionary(m_strMain);
    }
    m_aspell=optdlg.m_aspell;
    m_timeout=optdlg.m_time;
    spillDictionary.setMode(m_dictmode=optdlg.m_dictmode);
}

void SpillConfig::UpdateNames()
{
    playersForm pdlg;
    
    pdlg.addPlayers(m_players);
    pdlg.exec();
    m_players=pdlg.getPlayers();
}

QString SpillConfig::getPlayerName(int index)
{
    if (ComputersTurn(index))
	return m_players[index].left(m_players[index].length()-4);
    return m_players[index];
}
