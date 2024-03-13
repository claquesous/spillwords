#include "qstring.h"
#include "SpillDictionary.h"
#include "qfile.h"

SpillDictionary spillDictionary;

SpillDictionary::SpillDictionary()
{
#ifdef USE_ASPELL
    spell_config = new_aspell_config();

    AspellCanHaveError * possible_err = new_aspell_speller(spell_config);
    spell_checker = 0;
    if (aspell_error_number(possible_err) != 0)
      puts(aspell_error_message(possible_err));
    else
      spell_checker = to_aspell_speller(possible_err);
#endif // USE_ASPELL
}

SpillDictionary::~SpillDictionary()
{
#ifdef USE_ASPELL
    delete_aspell_config(spell_config);
    delete_aspell_speller(spell_checker); 
#endif // USE_ASPELL
}

#ifdef USE_ASPELL
bool SpillDictionary::setLanguage(QString language)
{
    return aspell_config_replace(spell_config, "lang", language)!=0;
}
#endif // USE_ASPELL

bool SpillDictionary::checkWord(QString word)
{
#ifdef USE_ASPELL
    if (mode==DICTIONARY_MODE_ASPELL)
      return aspell_speller_check(spell_checker, word, word.length())!=0;
#endif // USE_ASPELL
    return (m_dict.find(word)!=m_dict.end());
}

void SpillDictionary::UpdateDictionary(QString filename)
{
    QFile dict(filename);
    dict.open(IO_ReadOnly);
    QString next;

    m_dict=QStringList();
    while ( dict.readLine(next,50)>0 )
    {
	next.truncate(next.length()-1);
	m_dict.append(next.upper());
    }
    m_dict.sort();
}

void SpillDictionary::begin()
{
#ifdef USE_ASPELL
    if (mode==DICTIONARY_MODE_ASPELL)
    {
	      printf("Word list\n");
	      const AspellWordList * word_list = aspell_speller_main_word_list(spell_checker);
	wordEnum = aspell_word_list_elements(word_list);
      printf("Word list\n");
      return;
    }
#endif
    it=m_dict.begin();
}

QString SpillDictionary::next()
{
#ifdef USE_ASPELL
    if (mode==DICTIONARY_MODE_ASPELL)
    {
	QString word=aspell_string_enumeration_next(wordEnum);
	printf(word + "\n");
      return word;
  }
#endif
    return *(++it);
}

bool SpillDictionary::end()
{
#ifdef USE_ASPELL
    if (mode==DICTIONARY_MODE_ASPELL)
      return aspell_string_enumeration_at_end(wordEnum);
#endif // USE_ASPELL
    return (it==m_dict.end());
}

void SpillDictionary::setMode(int newMode)
{
    mode=newMode;
}
