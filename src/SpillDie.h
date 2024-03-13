/**
 * @file SpillDie.h
 *
 * @author Geoff Crew
 */
#ifndef SPILLDIE_H
#define SPILLDIE_H

#include <qwidget.h>

class SpillDie  : public QWidget
{
    Q_OBJECT
public:
    SpillDie( QWidget *parent, QString newfaces );
    void SetFace(char face);
    void SetSize(int newSize);
    char GetFace();
    void roll(bool newFace);
    QString getRightWord();
    void detachRightWord();
    QString getDownWord();
    void detachDownWord();
    static void setPlaying(bool playing);
    int getBlockScore();
    bool Attach();
    void SetPosition(QPoint pt);    
    // Used by SpillAI
    void attachRight(SpillDie *right);
    void attachLeft(SpillDie *left);
    void attachUp(SpillDie *up);
    void attachDown(SpillDie *down);
protected:
    void paintEvent( QPaintEvent * );
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
private:
    SpillDie* pLeft;
    SpillDie* pRight;
    SpillDie* pUp;
    SpillDie* pDown;    
    bool m_clicked;
    bool m_lock;
    QPoint m_diff;
    QPoint m_newpos;
    QChar m_face;
    QString m_faces;
    void MoveBlock(SpillDie* mover);    
    void DetachDie();    
    int blockSizeHelper();
    void shoveDice();
    void UnlockBlock();
    bool tryMoveLeft(SpillDie* mover);
    bool tryMoveDown(SpillDie* mover);
    bool trySetPosition(QPoint pt);
    static bool m_playing;
    void moveBlockHelper(SpillDie* mover);
    int blockScoreHelper();
    int blockSize();
};

#endif // SPILLDIE_H
