/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include "mainform.h"
#include "SpillSplash.h"
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include "images/splash.xpm"

mainForm *spillwords;

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // Shows a splash screen while loading mainForm
  SpillSplash wndSplash(splash);
  
  QTranslator tor( 0 );
  // set the location where your .qm files are in load() below as the last parameter instead of "."
  // for development, use "/" to use the english original as
  // .qm files are stored in the base project directory.
  tor.load( QString("spillwords.") + QTextCodec::locale(), "." );
  a.installTranslator( &tor );
  /* uncomment the following line, if you want a Windows 95 look*/
  //a.setStyle(WindowsStyle);
    
  spillwords = new mainForm();
  a.setMainWidget(spillwords);

  spillwords->show();
  wndSplash.close();  
  
  int retValue=a.exec();
  delete spillwords;
  return retValue;
}
