/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void spellForm::onYes()
{ // Return true, but don't add
    m_add=false;    
    accept();
}


void spellForm::onNo()
{ // Returns false
    reject();
}


void spellForm::onAdd()
{ // Return true and add
    m_add = true;
    accept();
}


void spellForm::setText( QString title, QString word )
{
    QString text = "Is " + word + " really a word?";
    setCaption(title);
    spellLabel->setText(text);
    spellLabel->adjustSize();
}
