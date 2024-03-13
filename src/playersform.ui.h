/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include "qmessagebox.h"

void playersForm::addPlayers(QStringList players)
{
    namesListBox->clear();
    namesListBox->insertStringList(players);
}

QStringList playersForm::getPlayers()
{
    return m_players;
}

void playersForm::onAdd()
{
    QString name = nameLineEdit->text();
    if (name!="")
    {
	if (name.endsWith(" (C)"))
	    QMessageBox::warning(this,"Illegal Player Name","Player names can not end with (C).","OK");
	else
	{
	    if (computerCheckBox->isChecked())
		name.append(" (C)");
	    namesListBox->insertItem( name );
	}
    }
    else
	QMessageBox::warning(this,"No Name","You must type a name.","OK");
}


void playersForm::onEdit()
{
    QString name = nameLineEdit->text();
    if (computerCheckBox->isChecked())
    {
	if (namesListBox->currentItem()>0)
	    name.append(" (C)");
	else
	    QMessageBox::warning(this,"First Player","First player can not be a computer player.","OK");
    }
    namesListBox->changeItem(name, namesListBox->currentItem());
}


void playersForm::onRemove()
{
    int index=namesListBox->currentItem();
    if (namesListBox->numRows()>2)
    {
	if (namesListBox->currentItem()==0 && namesListBox->item(1)->text().endsWith(" (C)"))
	    QMessageBox::warning(this,"First Player","First player can not be a computer player.","OK");
	else
	{
	    namesListBox->removeItem(index);
	    namesListBox->setSelected(index-1,true);
	}
    }
    else
	QMessageBox::warning(this,"Not Enough Players","You must always have at least two players.","OK");
}

void playersForm::accept()
{
    m_players.clear();
    for (int i=0; i<namesListBox->numRows(); i++)
	m_players.append(namesListBox->item(i)->text());
    QDialog::accept();
}

void playersForm::onToggle()
{
    if (namesListBox->currentItem()==0)
    	    QMessageBox::warning(this,"First Player","First player can not be a computer player.","OK");
    else
    {
	QString name = namesListBox->currentText();
	if (name.endsWith(" (C)"))
	    namesListBox->changeItem(name.left(name.length()-4), namesListBox->currentItem());
	else
	    namesListBox->changeItem(name+" (C)", namesListBox->currentItem());
    }
}
