####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = spillwords

## INCLUDES were found outside kdevelop specific part

spillwords_SOURCES = spellcheckform.ui aboutform.ui netform.ui messageform.ui mainform.ui spellform.ui optionsform.ui scoresform.ui SpillSplash.cpp SpillDie.cpp SpillConfig.cpp SpillBoard.cpp SpillAI.cpp playersform.ui main.cpp SpillDictionary.cpp cygwin_moc_types.cpp
SOURCES = SpillSplash.cpp SpillDie.cpp SpillConfig.cpp SpillBoard.cpp SpillAI.cpp main.cpp SpillDictionary.cpp cygwin_moc_types.cpp
INTERFACES = spellcheckform.ui aboutform.ui netform.ui messageform.ui mainform.ui spellform.ui optionsform.ui scoresform.ui playersform.ui 
TRANSLATIONS = 
KDE_OPTIONS = qtonly

spillwords_LDADD   = $(LIB_QPE) $(LIB_QT) $(LIBSOCKET) $(QT_LDADD) $(LIB_X11) $(ASPELL_LIBS)
spillwords_CXXFLAGS = $(CXXFLAGS) $(QT_CXXFLAGS)
AM_CPPFLAGS = $(QT_INCLUDES) 

EXTRA_DIST = images/spillwords.xpm images/splash.xpm images/net.xpm images/wood.xpm optionsform.ui.h playersform.ui.h scoresform.ui.h spellform.ui.h mainform.ui.h messageform.ui.h netform.ui.h spellcheckform.ui.h dict.dct click.wav SpillDictionary.h

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

spillwords_METASOURCES = AUTO

# the library search path. 
spillwords_LDFLAGS = $(all_libraries) $(QT_LDFLAGS) $(X_LDFLAGS) $(CYGWIN_LDFLAGS)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(spillwords_METASOURCES)


