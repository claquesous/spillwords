// SpillDie.cpp: implementation of the SpillDie class.
//
//////////////////////////////////////////////////////////////////////

#include "SpillDie.h"
#include "SpillBoard.h"
#include <qpainter.h>
#include "qbitmap.h"
#include <stdlib.h>				// rand() function

bool SpillDie::m_playing=false;

SpillDie::SpillDie( QWidget *parent, QString newfaces ) : QWidget( parent )
{    
    m_faces=newfaces;
    pDown=pLeft=pRight=pUp=NULL;
    m_clicked=m_lock=false;
 
    resize(25,25);
    setPaletteBackgroundColor(QColor(255,128,0));
}

void SpillDie::paintEvent( QPaintEvent * )
{
    QPainter p(this);
    QFont f;
    f.setPixelSize(width()/2);
    p.setFont(f);
    p.setPen( QPen( black,2 ) );
    p.drawText(0,0,width(),width(),Qt::AlignCenter,QString(m_face));
    QBitmap bm(size(),true);
    QPainter paint(&bm,true);
    paint.setBrush(color1);
    paint.drawRoundRect( 0,0,width(),width(),50,50 );
    setMask(bm);
}

void SpillDie::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_playing && e->button()==QMouseEvent::LeftButton)
    {
	m_clicked=false;
	if (Attach())
	{
	    // Play the click sound;
	    MoveBlock(NULL);
	}
    }
    QWidget::mouseReleaseEvent(e);
}

bool SpillDie::Attach()
{
    if (m_lock)
	return false;
    
    bool newAttach=false;
    
    // Make sure recursion doesn't repeat dice
    m_lock=true;
    
    if (pRight==NULL && (pRight=(SpillDie*)parentWidget()->childAt( pos() + QPoint(width()+width()/2,width()/2)))!=NULL)
    {	
	if (pRight->pLeft==NULL)	
	{
	    pRight->pLeft=this;
	    newAttach=true;
	}
	else
	    pRight=NULL;
    }
    if (pLeft==NULL && (pLeft=(SpillDie*)parentWidget()->childAt( pos() + QPoint(-width()/2,width()/2)))!=NULL)
    {
	if (pLeft->pRight==NULL)
	{
	    pLeft->pRight=this;
	    newAttach=true;
	}
	else
	    pLeft=NULL;
    }
    if (pUp==NULL && (pUp=(SpillDie*)parentWidget()->childAt( pos() + QPoint(width()/2,-width()/2)))!=NULL)
    {
	if (pUp->pDown==NULL)
	{
	    pUp->pDown=this;
	    newAttach=true;
	}
	else
	    pUp=NULL;
    }
    if (pDown==NULL && (pDown=(SpillDie*)parentWidget()->childAt( pos() + QPoint(width()/2,width()+width()/2)))!=NULL)
    {
	if (pDown->pUp==NULL)
	{
	    pDown->pUp=this;
	    newAttach=true;
	}
	else
	    pDown=NULL;
    }
    
    // Needs to return true if a dice attached to it needs to attach
    if (pUp!=NULL)
	newAttach|=pUp->Attach();
    if (pDown!=NULL)
	newAttach|=pDown->Attach();
    if (pLeft!=NULL)
	newAttach|=pLeft->Attach();
    if (pRight!=NULL)
	newAttach|=pRight->Attach();
    
    m_lock=false;
    
    // If any attachments have been made return true
    return newAttach;
}

void SpillDie::mousePressEvent(QMouseEvent *e)
{
    if (!m_playing);
    else if (e->button()==QMouseEvent::LeftButton)
    {
	m_clicked=true;
	m_diff = mapFromGlobal(e->globalPos());
    }
    else if (e->button()==QMouseEvent::RightButton)
    {
	DetachDie();
    }
    QWidget::mousePressEvent(e);
}

void SpillDie::mouseMoveEvent(QMouseEvent *e)
{
    //Calculates new position of Die based on difference from corner
    // Moves die and block with it
    if (m_playing && m_clicked )
    {
	if (!trySetPosition(m_newpos=pos()+mapFromGlobal(e->globalPos())-m_diff))
	{
	    if (m_newpos.x() <0)
		m_newpos.setX(0);
	    else if (m_newpos.x()+width() > parentWidget()->width())
		m_newpos.setX(parentWidget()->width()-width());
	    if (m_newpos.y()<0)
		m_newpos.setY(0);
	    else if (m_newpos.y()+width()>parentWidget()->height())
		m_newpos.setY(parentWidget()->height()-height());	    
	}
	bool lr,ud;
	lr = (m_newpos.x()!=pos().x() && tryMoveLeft(NULL));	
	ud = (m_newpos.y()!=pos().y() && tryMoveDown(NULL));
	if (lr || ud)
	{
	    if (!lr)
		m_newpos.setX(pos().x());
	    if (!ud)
		m_newpos.setY(pos().y());
	    MoveBlock(NULL);
	}
    }
    QWidget::mouseMoveEvent(e);
}

void SpillDie::DetachDie() 
{   
    if (pRight!=NULL)
    {// Detach to right
	pRight->pLeft=NULL;
	pRight=NULL;
    }
    if (pDown!=NULL)
    {// Detach down
	pDown->pUp=NULL;
	pDown=NULL;
    }
    if (pUp!=NULL)
    {// Detach up
	pUp->pDown=NULL;
	pUp=NULL;
    }
    if (pLeft!=NULL)
    {// Detach to left
	pLeft->pRight=NULL;
	pLeft=NULL;
    }
}

void SpillDie::roll(bool newFace)
{ // Set a random face
    QSize boardSize=parentWidget()->size();
    if (newFace)
	m_face=m_faces[rand()%6];
    pLeft=pRight=pUp=pDown=NULL;
    SetPosition(QPoint(rand()%(boardSize.width()-width()),rand()%(boardSize.height()-width())));
    shoveDice();
    repaint();
}

char SpillDie::GetFace()
{ // Return face
    return m_face;
}

void SpillDie::SetSize(int newSize)
{ 
    // Set dice width
    resize(newSize,newSize);
    MoveBlock(NULL);
    repaint();
}

void SpillDie::SetFace(char face)
{ // Set dice face
    m_face=face;
    pLeft=pRight=pUp=pDown=NULL;
    repaint();
}

bool SpillDie::trySetPosition(QPoint pt)
{
    // Don't let the dice go off the screen
    if (pt.x() <0 || pt.x()+width() > parentWidget()->width() || pt.y()<0 || pt.y()+width()>parentWidget()->height())
	return false;
    else 
	m_newpos=pt;
    return true;
}

void SpillDie::SetPosition(QPoint pt)
{
    // Don't let the dice go off the screen
    if (pt.x() <0)
	pt.setX(0);
    else if (pt.x()+width() > parentWidget()->width())
	pt.setX(parentWidget()->width()-width());
    
    if (pt.y()<0)
	pt.setY(0);
    else if (pt.y()+width()>parentWidget()->height())
	pt.setY(parentWidget()->height()-height());   

    m_newpos=pt;
    move(pt);
}

void SpillDie::MoveBlock(SpillDie* mover)
{
    moveBlockHelper(mover);
    UnlockBlock();
}

void SpillDie::shoveDice()
{
    SpillDie *pShove=NULL;
    if ( pLeft==NULL && m_newpos.x() <= pos().x() &&
	 ( (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(-1,2)))!=NULL ||
	   (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(-1,width()-2)))!=NULL)
	&& !pShove->m_lock)
    {
	pShove->m_newpos=(QPoint(m_newpos.x()-width(),pShove->pos().y()));
	if (pShove->m_newpos!=pShove->pos())
	    pShove->MoveBlock(NULL);
    }
    else if ( pRight==NULL && m_newpos.x() >= pos().x() &&
	      ( (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width(),2)))!=NULL ||
		(pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width(),width()-2)))!=NULL)
	&& !pShove->m_lock)
    {
	pShove->m_newpos=(QPoint(m_newpos.x()+width(),pShove->pos().y()));
	if (pShove->m_newpos!=pShove->pos())	
	    pShove->MoveBlock(NULL);
    }

    if ( pUp==NULL && m_newpos.y() <= pos().y() &&
	 ( (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(2,-1)))!=NULL ||
	   (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width()-2,-1)))!=NULL)
	&& !pShove->m_lock)
    {
	pShove->m_newpos=(QPoint(pShove->pos().x(),m_newpos.y()-width()));
	if (pShove->m_newpos!=pShove->pos())	
	    pShove->MoveBlock(NULL);
    }
    else if ( pDown==NULL && m_newpos.y() >= pos().y() &&
	      ( (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(2,width())))!=NULL ||
		(pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width()-2,width())))!=NULL)
	&& !pShove->m_lock)
    {
	pShove->m_newpos=(QPoint(pShove->pos().x(),m_newpos.y()+width()));
	if (pShove->m_newpos!=pShove->pos())	
	    pShove->MoveBlock(NULL);
    }    
}

// Move all dice connected to a specific die
void SpillDie::moveBlockHelper(SpillDie* mover)
{
    if (m_lock)
	return;
    
    // Also needs locks to prevent recursive loops
    m_lock=true;
    
    if (mover!=NULL)
    {// Move blocks on all sides towards the mover block
	if (mover->pDown==this)
	    m_newpos=(mover->m_newpos + QPoint(0,width()));
	else if (mover->pUp==this)
	    m_newpos=(mover->m_newpos + QPoint(0,-width()));
	else if (mover->pLeft==this)
	    m_newpos=(mover->m_newpos + QPoint(-width(),0));
	else if (mover->pRight==this)
	    m_newpos=(mover->m_newpos + QPoint(width(),0));
    }
    
    // Push nearby dice around.
    shoveDice();
    
    // Recursively call all blocks to move towards original mover in an order that prevents overlaps(/flicker)
    if (m_newpos.x() > pos().x())
    {
	if (pRight!=mover && pRight!=NULL)
	    pRight->moveBlockHelper(this);
	if ( m_newpos.y() > pos().y() )
	{
	    if (pDown!=mover && pDown!=NULL)
		pDown->moveBlockHelper(this);
	    SetPosition(m_newpos);		    
	    if (pUp!=mover && pUp!=NULL)
		pUp->moveBlockHelper(this);		    
	}
	else
	{
	    if (pUp!=mover && pUp!=NULL)
		pUp->moveBlockHelper(this);
	    SetPosition(m_newpos);
	    if (pDown!=mover && pDown!=NULL)
		pDown->moveBlockHelper(this);	    
	}
	if (pLeft!=mover && pLeft!=NULL)
	    pLeft->moveBlockHelper(this);
    }
    else
    {
	if (pLeft!=mover && pLeft!=NULL)
	    pLeft->moveBlockHelper(this);
	if ( m_newpos.y() > pos().y() )
	{
	    if (pDown!=mover && pDown!=NULL)
		pDown->moveBlockHelper(this);
	    SetPosition(m_newpos);		    
	    if (pUp!=mover && pUp!=NULL)
		pUp->moveBlockHelper(this);		    
	}
	else
	{
	    if (pUp!=mover && pUp!=NULL)
		pUp->moveBlockHelper(this);
	    SetPosition(m_newpos);		    
	    if (pDown!=mover && pDown!=NULL)
		pDown->moveBlockHelper(this);	    
	}	
	if (pRight!=mover && pRight!=NULL)
	    pRight->moveBlockHelper(this);	
    }
}

// Move all dice connected to a specific die
bool SpillDie::tryMoveDown(SpillDie* mover)
{
    if (m_lock)
	return true;
    
    // Also needs locks to prevent recursive loops
    m_lock=true;
    
    if (mover!=NULL)
    {// Move blocks on all sides towards the mover block
	if (mover->pDown==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(0,width())))
		return m_lock=false;
	}
	else if (mover->pUp==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(0,-width())))
		return m_lock=false;
	}
	else if (mover->pLeft==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(-width(),0)))
		return m_lock=false;
	}
	else if (mover->pRight==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(width(),0)))
		return m_lock=false;
	}
    }

    // Check if we've moved a dice width
    if (pUp==NULL && m_newpos.y()<=pos().y()-width()
	&& ( parentWidget()->childAt(pos()+QPoint(0,-width()+1))!=NULL
	     || parentWidget()->childAt(pos()+QPoint(width(),-width()+1))!=NULL))
	return m_lock=false;
    if (pDown==NULL && m_newpos.y()>=pos().y()+width()
	&& ( parentWidget()->childAt(pos()+QPoint(0,2*width()-1))!=NULL
	     || parentWidget()->childAt(pos()+QPoint(width(),2*width()-1))!=NULL))
	return m_lock=false;
    
    SpillDie *pShove=NULL;
    if ( pUp==NULL && m_newpos.y() < pos().y()  &&
	 ((pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(2,-1)))!=NULL ||
	  (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width()-2,-1)))!=NULL))
    { 
	if (!pShove->trySetPosition(QPoint(pShove->pos().x(),m_newpos.y()-width())) || !pShove->tryMoveDown(NULL))
	     return m_lock=false;
     }
    else if ( pDown==NULL && m_newpos.y() > pos().y() &&
	      ((pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(2,width())))!=NULL ||
	       (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width()-2,width())))!=NULL))
    {
	if (!pShove->trySetPosition(QPoint(pShove->pos().x(),m_newpos.y()+width())) || !pShove->tryMoveDown(NULL))
	    return m_lock=false;
    }
    
    // Recursively call all blocks to move towards original mover
    if (pDown!=mover && pDown!=NULL && !pDown->tryMoveDown(this))
	return m_lock=false;
    if (pUp!=mover && pUp!=NULL && !pUp->tryMoveDown(this))
	return m_lock=false;
    if (pRight!=mover && pRight!=NULL && !pRight->tryMoveDown(this))
	return m_lock=false;
    if (pLeft!=mover && pLeft!=NULL && !pLeft->tryMoveDown(this))
	return m_lock=false;
    
    m_lock=false;
    return true;
}

// Move all dice connected to a specific die
bool SpillDie::tryMoveLeft(SpillDie* mover)
{
    if (m_lock)
	return true;
    
    // Also needs locks to prevent recursive loops
    m_lock=true;
    
    if (mover!=NULL)
    {// Move blocks on all sides towards the mover block
	if (mover->pDown==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(0,width())))
		return m_lock=false;
	}
	else if (mover->pUp==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(0,-width())))
		return m_lock=false;
	}
	else if (mover->pLeft==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(-width(),0)))
		return m_lock=false;
	}
	else if (mover->pRight==this)
	{
	    if (!trySetPosition(mover->m_newpos + QPoint(width(),0)))
		return m_lock=false;
	}
    }
    
    // Check if we've moved over a dice width
    if (pLeft==NULL && m_newpos.x()<=pos().x()-width() 
	&& ( parentWidget()->childAt(pos()+QPoint(-width()+1,0))!=NULL
	     || parentWidget()->childAt(pos()+QPoint(-width()+1,width()))!=NULL))
	return m_lock=false;
    if (pRight==NULL && m_newpos.x()>=pos().x()+width() 
	&& ( parentWidget()->childAt(pos()+QPoint(2*width()-1,0))!=NULL 
	     || parentWidget()->childAt(pos()+QPoint(2*width()-1,width()))!=NULL))
	return m_lock=false;
    
    // Push nearby dice around.
    SpillDie *pShove=NULL;
    if ( pLeft==NULL && m_newpos.x() < pos().x() &&
	 ( (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(-1,2)))!=NULL ||
	   (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(-1,width()-2)))!=NULL))
    {
	if (!pShove->trySetPosition(QPoint(m_newpos.x()-width(),pShove->pos().y())) || !pShove->tryMoveLeft(NULL))
	    return m_lock=false;
    }
    else if ( pRight==NULL && m_newpos.x() > pos().x() &&
	      ( (pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width(),2)))!=NULL ||
		(pShove=(SpillDie*)parentWidget()->childAt(m_newpos + QPoint(width(),width()-2)))!=NULL))
    {
	if (!pShove->trySetPosition(QPoint(m_newpos.x()+width(),pShove->pos().y())) || !pShove->tryMoveLeft(NULL))
	    return m_lock=false;
    }
    
    // Recursively call all blocks to move towards original mover
    if (pRight!=mover && pRight!=NULL && !pRight->tryMoveLeft(this))
	return m_lock=false;
    if (pLeft!=mover && pLeft!=NULL && !pLeft->tryMoveLeft(this))
	return m_lock=false;
    if (pUp!=mover && pUp!=NULL && !pUp->tryMoveLeft(this))
	return m_lock=false;
    if (pDown!=mover && pDown!=NULL && !pDown->tryMoveLeft(this))
	return m_lock=false;
    
    m_lock=false;
    return true;
}

// Some recursive functions need to keep the lock on to make sure
// no dice runs twice.  This will unlock everything.
void SpillDie::UnlockBlock()
{
    // Don't unlock if already unlocked
    if (m_lock==false)
	return;
    
    m_lock=false;
    if (pDown!=NULL)
	pDown->UnlockBlock();
    if (pUp!=NULL)
	pUp->UnlockBlock();
    if (pLeft!=NULL)
	pLeft->UnlockBlock();
    if (pRight!=NULL)
	pRight->UnlockBlock();
}

void SpillDie::setPlaying(bool playing)
{
    m_playing=playing;
}

int SpillDie::blockSize()
{
    int size=blockSizeHelper();
    UnlockBlock();
    return size;
}

int SpillDie::blockSizeHelper()
{// Calculates the size of the block
    if (m_lock)
	return 0;
    
    int size=1;
    // This lock can't be unlocked must unlock after done
    m_lock=true;
    
    if (pDown!=NULL)
	size+=pDown->blockSizeHelper();
    if (pRight!=NULL)
	size+=pRight->blockSizeHelper();
    if (pUp!=NULL)
	size+=pUp->blockSizeHelper();
    if (pLeft!=NULL)
	size+=pLeft->blockSizeHelper();
    
    return size;
}

QString SpillDie::getRightWord()
{
    QString word="";
    if (pLeft!=NULL || pRight==NULL)
	return word;
    
    for (SpillDie *pTemp=this; pTemp!=NULL; pTemp=pTemp->pRight)
	word.append(pTemp->GetFace());
    
    return word;
}

void SpillDie::detachRightWord()
{
    for (SpillDie *pTemp=pRight; pTemp!=NULL; pTemp=pTemp->pRight)
    {
	pTemp->pLeft->pRight=NULL;
	pTemp->pLeft=NULL;
    }
}

QString SpillDie::getDownWord()
{
    QString word="";
    if (pUp!=NULL || pDown==NULL)
	return word;
    
    for (SpillDie *pTemp=this; pTemp!=NULL; pTemp=pTemp->pDown)
	word.append(pTemp->GetFace());
    
    return word;
}

void SpillDie::detachDownWord()
{
    for (SpillDie *pTemp=pDown; pTemp!=NULL; pTemp=pTemp->pDown)
    {
	pTemp->pUp->pDown=NULL;
	pTemp->pUp=NULL;
    }
}

int SpillDie::getBlockScore()
{
    int score = blockScoreHelper();
    UnlockBlock();
    
    int unused= 15-blockSize();
    if (unused==14)
	unused=15;

    return score - (unused*unused);
}

int SpillDie::blockScoreHelper()
{
    if (m_lock)
	return 0;
    
    m_lock=true;
    
    int score=0;
    
    // Does dice start a word?
    int length;
    length=getRightWord().length(); 
    score+=length*length;	
    length=getDownWord().length(); 
    score+=length*length;
    
    if (pUp!=NULL)
	score+=pUp->blockScoreHelper();
    if (pDown!=NULL)
	score+=pDown->blockScoreHelper();
    if (pRight!=NULL)
	score+=pRight->blockScoreHelper();
    if (pLeft!=NULL)
	score+=pLeft->blockScoreHelper();
    
    return score;
}

void SpillDie::attachRight(SpillDie *right) 
{ 
    right->SetPosition(pos()+QPoint(width(),0));
    pRight = right; 
    pRight->pLeft=this;
    right->MoveBlock(NULL);    
}

void SpillDie::attachLeft(SpillDie *left) 
{
    left->SetPosition(pos()-QPoint(width(),0));
    pLeft = left;
    pLeft->pRight=this;
    left->MoveBlock(NULL);    
}
    
void SpillDie::attachUp(SpillDie *up) 
{
    up->SetPosition(pos()-QPoint(0,width()));
    pUp = up;
    pUp->pDown=this;
    up->MoveBlock(NULL);    
}

void SpillDie::attachDown(SpillDie *down) 
{ 
    down->SetPosition(pos()+QPoint(0,width()));
    pDown = down;
    pDown->pUp=this;
    down->MoveBlock(NULL);    
}
