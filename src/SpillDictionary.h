#ifndef SPILLDICTIONARY_H
#define SPILLDICTIONARY_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#ifdef USE_ASPELL
#include "aspell.h"
#endif // USE_ASPELL
#endif // HAVE_CONFIG_H

class QString;
#include "qstringlist.h"

#define DICTIONARY_MODE_ASPELL 	0x5
#define DICTIONARY_MODE_FILE		0x3

class SpillDictionary
{
public:
    SpillDictionary();
    ~SpillDictionary();
#ifdef USE_ASPELL
    bool setLanguage(QString);
#endif // USE_ASPELL
    void UpdateDictionary(QString filename);
    bool checkWord(QString);
    void begin();
    QString next();
    bool end();
    void setMode(int newMode);
private:
#ifdef USE_ASPELL
    AspellConfig * spell_config;
    AspellSpeller * spell_checker;
    AspellStringEnumeration * wordEnum;
#endif // USE_ASPELL
    QStringList m_dict;
    QStringList::Iterator it;
    int mode;
};

extern SpillDictionary spillDictionary;
#endif // SPILLDICTIONARY_H
