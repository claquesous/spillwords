/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void scoresForm::onClear()
{
    m_cleared=true;
    scoresListBox->clear();
    m_scores.clear();
    m_numscores=0;
    totalTextLabel->setText("0");
}

void scoresForm::onPlayer()
{
    scoresListBox->clear();
    int i;
    int total=0;
    for (i=playersComboBox->currentItem(); i<m_numscores; i+=m_players.size())
    {
	total+=m_scores[i];
	scoresListBox->insertItem(QString("%1").arg(m_scores[i]));
    }
    totalTextLabel->setText(QString("%1").arg(total));
}

void scoresForm::exec()
{
    m_cleared=false;
    scoresListBox->clear();
    playersComboBox->insertStringList(m_players);
    playersComboBox->setCurrentItem(0);
    onPlayer();
    
    QDialog::exec();
}
