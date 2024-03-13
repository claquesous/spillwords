/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include <stdlib.h>
#include "SpillDictionary.h"

void optionsForm::exec()
{
    intelLineEdit->setText(QString("%1").arg(m_ai));
    accLineEdit->setText(QString("%1").arg(m_aiacc));
    endLineEdit->setText(QString("%1").arg(m_endscore));
    
    watchRadioButton->setChecked(m_watch);
    sameCheckBox->setChecked(m_same);
    sameCheckBox->setEnabled(!m_nosame);
    
    timeSpinBox->setValue(m_time);
    
    mainLineEdit->setText(m_strMain);
    aspellLineEdit->setText(m_aspell);
 
#ifdef USE_ASPELL
    aspellRadioButton->setEnabled(true);
    if (m_dictmode==DICTIONARY_MODE_ASPELL)
    {
      aspellRadioButton->setChecked(true);
	onAspell();
    }
#endif // USE_ASPELL
    QDialog::exec();
}

void optionsForm::onMain()
{
    m_strMain = QFileDialog::getOpenFileName(".", "Dictionary Files (*.dct)", this, "Main Dictionary","Choose a Dictionary");
    mainLineEdit->setText(m_strMain);
}

void optionsForm::onOk() 
{
    m_ai=atoi(intelLineEdit->text());
    m_aiacc=atoi(accLineEdit->text());
    m_endscore=atoi(endLineEdit->text());
    m_time=timeSpinBox->value();
    accept();
}

void optionsForm::onSimul() 
{
    // Flip with netwatch
    m_watch=false;
}

void optionsForm::onWatch() 
{
    // Flip with netsimul
    m_watch=true;
}

void optionsForm::onSame()
{
    m_same=!m_same;
}

void optionsForm::onAspell()
{
    mainLineEdit->setEnabled(false);
    aspellLineEdit->setEnabled(true);
    m_dictmode=DICTIONARY_MODE_ASPELL;
}

void optionsForm::onFile()
{
    mainLineEdit->setEnabled(true);
    aspellLineEdit->setEnabled(false);
    m_dictmode=DICTIONARY_MODE_FILE;
}
