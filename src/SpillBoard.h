// SpillGame.h: interface for the SpillBoard class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SPILLBOARD_H
#define SPILLBOARD_H

#include "qstring.h"
#include "qwidget.h"

class SpillDie;
class QSocket;
class SpillServerSocket; // Need to make my own Server because QServerSocket doesn't signal

class SpillBoard  : public QWidget
{
    Q_OBJECT    
public:
    // General functions    
    SpillBoard();
    virtual ~SpillBoard();
    int ScoreGame(QString scorer);
    bool QNoU();
    void BeginGame();
    void bestPuzzle(int intelligence, int accuracy);
    void CopyDice(SpillBoard* game);
    // Net functions
    void Accept(int socket);    
    void NetSendScore(int score);
    void NetSendDice();
    void NetBeginGame();
    void NetEndGame();
    void Send(QString netmsg);    
    void StartServer(int port);
    void StartClient(QString address,int port);
    bool Connected(void);
public slots:
    // Called by socket
    void Receive();
    void Close();    
protected:
    void resizeEvent(QResizeEvent *);
private:
    SpillDie* Dice[15];			// The dice
    QSocket *m_socket;			// Socket
    SpillServerSocket *m_tempserve;		// Temporary server socket
    bool UserSpellCheck(QString strWord, QString scorer);
};

#endif // SPILLBOARD_H
