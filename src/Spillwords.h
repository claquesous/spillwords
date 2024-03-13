#ifndef SPILLWORDS_H
#define SPILLWORDS_H

class mainForm;

mainForm *getMainWindow()
{
    extern mainForm *spillwords;
    return spillwords;
}

#endif
