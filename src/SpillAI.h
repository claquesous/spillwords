#ifndef SPILLAI_H
#define SPILLAI_H

class SpillDie;
#include "qstringlist.h"

class SpillAI
{
public:
    SpillAI(SpillDie *dice[15], int intelligence, int accuracy);
    void getBestPuzzle();
private:
    int m_intel;
    int m_acc;
    SpillDie *dice[15];
    QStringList getWords(QString letters, bool overlap, int maxLength=-1);
    int getMaxFirstWord();
    int getMaxSecondWord(int w1length);
    int bestScore;
    bool rebuild(QString w1, QString w2="", int rinter=0, int dinter=0, QString w3="", bool leftright=true, int finter=0, int tinter=0);
};

#endif
