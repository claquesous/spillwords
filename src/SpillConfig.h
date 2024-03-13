/**
 * @author Geoff Crew
 */
#ifndef SPILLCONFIG_H
#define SPILLCONFIG_H

#include "qstring.h"
#include "qstringlist.h"
#include "qvaluelist.h"

class SpillConfig
{
public: 
    SpillConfig(); // create from serialization only
    virtual ~SpillConfig();
    bool DisplayScores();
    void UpdateOptions(void);
    bool FirstPlayersTurn(void);
    void AddScore(int newScore);
    void UpdateNames();
    QString GetCurrentPlayerName(void);
    QString getMainDictionary() { return m_strMain; }
    QString getPlayerName(int index);
    void SetPlayerName(QString newName, int player);
    void SwapPlayerNames(void);
    bool ComputersTurn(int player);
    bool isNetWatch() { return m_netwatch;}
    bool isSame() { return m_same;} 
    void setNetWatch(bool netwatch) { m_netwatch=netwatch; }
    void setSame(bool same) { m_same=same; }
    bool endOfRound();
    int getAI() { return m_ai;}
    int getAcc() { return m_aiacc; }
    int getNumPlayers() { return m_players.size();}
    int getPlayerIndex() { return m_numscores%m_players.size(); }
    int timeOut() { return m_timeout; }
    void resetScores();
    void resetTime() { m_timeout=3; }
private:
    bool GameOver(void);
    QValueList<int> m_scores;
    QStringList m_players;
    QString m_aspell;
    QString m_strMain;
    unsigned int m_numscores;
    bool finished;
    bool m_netwatch;			// Net player watch mode
    bool m_same;				// Same mode in tournament
    int m_ai;				// AI intelligence level
    int m_aiacc;				// AI accuracy
    int m_timeout;				// Number of minutes per round
    int m_endscore;				// Tournament end score
    int m_dictmode;			// Dictionary mode
};

#endif // SPILLCONFIG_H
