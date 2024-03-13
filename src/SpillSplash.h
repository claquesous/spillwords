/**
 * @file SpillSplash.h
 *
 * @author Geoff Crew
 */
#ifndef SPILLSPLASH_H
#define SPILLSPLASH_H
 
#include <qpixmap.h>
#include <qwidget.h>

/**
 * SpillSplash window
 */
class SpillSplash : public QWidget
{
public:
    SpillSplash( const QPixmap &pixmap );
    void repaint();
};


#endif // SPILLSPLASH_H
