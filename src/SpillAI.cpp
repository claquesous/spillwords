#include "SpillAI.h"
#include "SpillDie.h"
#include "SpillDictionary.h"

int SpillAI::getMaxFirstWord()
{
    for (int i=15; i>2; i--)
    {
	if ((i*i-((15-i)*(15-i)))<=(m_intel+m_acc))
	    return i;
    }
    return 2;
}

int SpillAI::getMaxSecondWord(int w1length)
{
    int max;
    if (15-w1length<w1length)
	max=15-w1length;
    else
	max=w1length;
    for (int i=max; i>2; i--)
    {
	if ((w1length*w1length+i*i-((16-w1length-i)*(16-w1length-i)))<=(m_intel+m_acc))
	    return i;
    }
    return 2;
}
 
SpillAI::SpillAI(SpillDie *dice[15], int intelligence, int accuracy)
{
    for (int i=0; i<15; i++)
	this->dice[i]=dice[i];
    m_intel = intelligence;
    m_acc = accuracy;
}

QStringList SpillAI::getWords(QString letters, bool overlap, int maxLength)
{
    QString bestword="";
    unsigned int i,j;
    QString tempword;
    QString currentword;
    bool used[15];
    bool possible;
    QStringList words;
    unsigned int maxword=0;
    
    if (maxLength==-1)
	maxLength=letters.length();
    
    for (spillDictionary.begin(); !spillDictionary.end(); )
    {
	for (i=0; i<15; i++)
	    used[i]=false;
	possible=true;
	// Is next word bigger than biggest so far?
	tempword= spillDictionary.next().upper();
	currentword = tempword;
	
	if (maxword<=tempword.length() && maxLength>=(int)tempword.length())
	{
	    QString available=letters;
	    for (i=0; i<tempword.length() && possible; i++)
	    {
		possible=false;
		for (j=0; j<letters.length() && !possible; j++)
		{
		    if (tempword[i]==letters[j] && !used[j])
		    {
			possible=true;
			used[j]=true;
		    }
		}
	    }
	    // All letters were found so new longest word (also guarantees first letter overlaps if needed)
	    if (possible && (!overlap || used[0]))
	    {
		if (tempword.length()>maxword)
		{
		    maxword=tempword.length();
		    words.clear();
		}
		words<<tempword;
	    }
	}
    }
    return words;
 }

// Constructs the best puzzle
bool SpillAI::rebuild(QString w1, QString w2, int rinter, int dinter, QString w3, bool leftright, int finter, int tinter)
{
    int wordscore=w1.length()*w1.length()+w2.length()*w2.length()+w3.length()*w3.length();
    int unused=15-w1.length();
    if (w2.length()!=0)
	unused-=w2.length()-1;
    if (w3.length()!=0)
	unused-=w3.length()-1;
    wordscore-=unused*unused;
    
    if ((wordscore>=m_intel-m_acc) || (w1+w2==16 && wordscore<m_intel-m_acc))
	bestScore=0;
    else if (wordscore<=m_intel-m_acc && m_intel-m_acc-wordscore<bestScore)
	bestScore=m_intel-m_acc-wordscore;
    else if 	(wordscore>=m_intel+m_acc && wordscore-m_intel-m_acc<bestScore)
	bestScore=wordscore-m_intel-m_acc;
    else
	return false;

    bool used[15];
    for (int i=0; i<15; i++)
    {
	used[i]=false;
	dice[i]->roll(false);
    }
    
    SpillDie *last=NULL;
    SpillDie *lrinter=NULL;
    SpillDie *thirdinter=NULL;
    for (int i=0; i<(int)w1.length(); i++)
    {
	for (int j=0; j<15; j++)
	{
	    if (w1[i]==dice[j]->GetFace() && !used[j])
	    {
		if (last!=NULL)
		    last->attachRight(dice[j]);
		if (i==rinter)
		    lrinter=dice[j];
		if (i==finter && leftright)
		    thirdinter=dice[j];
		last=dice[j];
		used[j]=true;
		break;
	    }
	}
    }
    if (w2!="") // Rebuild Second Word
    {
	last=lrinter;
	for (int i=dinter+1; i<(int)w2.length(); i++)
	{
	    for (int j=0; j<15; j++)
	    {
		if (w2[i]==dice[j]->GetFace() && !used[j])
		{
		    last->attachDown(dice[j]);
		    if (i==finter && !leftright)
			thirdinter=dice[j];
		    last=dice[j];
		    used[j]=true;
		    break;
		}
	    }
	}
	last=lrinter;
	for (int i=dinter-1; i>=0; i--)
	{
	    for (int j=0; j<15; j++)
	    {
		if (w2[i]==dice[j]->GetFace() && !used[j])
		{
		    last->attachUp(dice[j]);
		    if (i==finter && !leftright)
			thirdinter=dice[j];
		    last=dice[j];
		    used[j]=true;
		    break;
		}
	    }
	}
    }
    if (w3!="") // Rebuild third word
    {
	if (leftright)
	{
	    last=thirdinter;
	    for (int i=tinter+1; i<(int)w3.length(); i++)
	    {
		for (int j=0; j<15; j++)
		{
		    if (w3[i]==dice[j]->GetFace() && !used[j])
		    {
			last->attachDown(dice[j]);
			last=dice[j];
			used[j]=true;
			break;
		    }
		}
	    }
	    last=thirdinter;
	    for (int i=tinter-1; i>=0; i--)
	    {
		for (int j=0; j<15; j++)
		{
		    if (w3[i]==dice[j]->GetFace() && !used[j])
		    {
			last->attachUp(dice[j]);
			last=dice[j];
			used[j]=true;
			break;
		    }
		}
	    }
	}
	else // Third word intersects with second word
	{
	    last=thirdinter;
	    for (int i=tinter+1; i<(int)w3.length(); i++)
	    {
		for (int j=0; j<15; j++)
		{
		    if (w3[i]==dice[j]->GetFace() && !used[j])
		    {
			last->attachRight(dice[j]);
			last=dice[j];
			used[j]=true;
			break;
		    }
		}
	    }
	    last=thirdinter;
	    for (int i=tinter-1; i>=0; i--)
	    {
		for (int j=0; j<15; j++)
		{
		    if (w3[i]==dice[j]->GetFace() && !used[j])
		    {
			last->attachLeft(dice[j]);
			last=dice[j];
			used[j]=true;
			break;
		    }
		}
	    }	    
	}
    }
    return bestScore==0;
}

// Finds the best three word puzzle
void SpillAI::getBestPuzzle()
{
    bestScore=m_intel-m_acc+225;
    int i,j;
    unsigned int maxSecond=2, maxThird=2; // Don't want to look at 1 letter words
    QStringList bestPuzzle;
    QString letters;
    for (i=0; i<15; i++)
	letters.append(dice[i]->GetFace());
    QStringList bestWords=getWords(letters,false, getMaxFirstWord());
    for (QStringList::Iterator it=bestWords.begin(); it!=bestWords.end(); it++)
    {
	QString word=*it;
	if (maxSecond==2 && maxThird==2 && rebuild(word))
	    return;
	QString leftovers=letters;	
	for (i=0; i<(int)word.length(); i++)
	    leftovers.remove(leftovers.find(word[i]),1);
	for (i=0; i<(int)word.length(); i++)
	{
	    int lrintersection=i;

	    QStringList secondWords=getWords(word[i]+leftovers,true, getMaxSecondWord(word.length()));
	    if (secondWords.size()==0 || secondWords[0].length()<maxSecond)
		continue;
	    if (secondWords[0].length()>maxSecond)
		maxThird=2;
	    maxSecond=secondWords[0].length();
	    if (maxThird==2 && rebuild(word, secondWords[0], lrintersection, secondWords[0].find(word[lrintersection])))
		return;
	    for (QStringList::Iterator it2=secondWords.begin(); it2!=secondWords.end(); it2++)
	    {
		QString newLeftovers=leftovers;
		QString secondWord=*it2;
		int udintersection=secondWord.find(word[lrintersection]);
		for (j=0; j<(int)secondWord.length(); j++)
		{
		    if (j!=udintersection)
			newLeftovers.remove(newLeftovers.find(secondWord[j]),1);
		}
		// Now search for all words that are still available with one overlap between first two words
		for (j=0; j<(int)word.length(); j++)
		{
		    if (j!=lrintersection)
		    {
			int trinter=j; // third right intersection			
			QStringList finalWords=getWords(word[j]+newLeftovers,true, secondWord.length());
			if (finalWords.size()==0 || finalWords[0].length()<maxThird)
			    continue;
			maxThird=finalWords[0].length();
			// Just need to find a word that fits in the puzzle
			for (QStringList::Iterator it3=finalWords.begin(); it3!=finalWords.end(); it3++)
			{
			    QString word3=*it3;
			    int tdinter=word3.find(word[trinter]);
			    if (!((j>=lrintersection-1 && j<=lrintersection+1) && ((udintersection>0 && tdinter>0) 
				|| (udintersection<((int)secondWord.length()-1) && tdinter<((int)word3.length()-1)))) )
			    {
				if (rebuild(word,secondWord,lrintersection,udintersection,word3,true,trinter,tdinter))
				    return;
				break;
			    }
			}
		    }
		}
		for (j=0; j<(int)secondWord.length(); j++)
		{
		    if (j!=udintersection)
		    {
			int tdinter=j; // third down intersection			
			QStringList finalWords=getWords(secondWord[j]+newLeftovers,true, secondWord.length());
			if (finalWords.size()==0 || finalWords[0].length()<maxThird)
			    continue;
			maxThird=finalWords[0].length();
			// Just need to find a word that fits in the puzzle
			for (QStringList::Iterator it3=finalWords.begin(); it3!=finalWords.end(); it3++)
			{
			    QString word3=*it3;
			    int trinter=word3.find(secondWord[tdinter]);
			    if (!((j>=udintersection-1 && j<=udintersection+1) && ((lrintersection>0 && trinter>0)
				|| (lrintersection<((int)word.length()-1) && trinter<((int)word3.length()-1)))))
			    {
				if (rebuild(word,secondWord,lrintersection,udintersection,word3,false,tdinter,trinter))
				    return;
				break;
			    }
			}
		    }
		}
	    }
	}
    }
}
