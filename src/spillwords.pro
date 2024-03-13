SOURCES	+= main.cpp \
	SpillDie.cpp \
	SpillBoard.cpp \
	SpillConfig.cpp \
	SpillSplash.cpp \
	SpillAI.cpp \
	cygwin_moc_types.cpp \
	SpillDictionary.cpp
HEADERS	+= SpillDie.h \
	SpillBoard.h \
	SpillConfig.h \
	SpillSplash.h \
	SpillAI.h \
	Spillwords.h \
	SpillDictionary.h
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= mainform.ui \
	messageform.ui \
	aboutform.ui \
	spellform.ui \
	netform.ui \
	optionsform.ui \
	playersform.ui \
	scoresform.ui \
	spellcheckform.ui
IMAGES	= images/net.xpm \
	images/spillwords.xpm \
	images/splash.xpm \
	images/wood.xpm
TEMPLATE	=app
CONFIG	+= qt warn_on release
LANGUAGE	= C++
