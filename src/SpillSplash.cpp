// SpillSplash.cpp : implementation file
//

#include "SpillSplash.h"
#include <qapplication.h>
#include <qpixmap.h>

SpillSplash::SpillSplash( const QPixmap &pixmap ) : QWidget( 0, 0, WStyle_Customize | WStyle_Splash )
{
    setErasePixmap( pixmap );
    resize( pixmap.size() );
    QRect scr = QApplication::desktop()->screenGeometry();
    move( scr.center() - rect().center() );
    show();
    repaint();
}

void SpillSplash::repaint()
{
    QWidget::repaint();
    QApplication::flush();
}
