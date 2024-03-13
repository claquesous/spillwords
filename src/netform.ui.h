/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
void netForm::init()
{
    m_server=false;
}

void netForm::onServerToggle(bool toggled)
{
    addressLineEdit->setEnabled(!toggled);
    m_server=toggled;
}


void netForm::onOk()
{
    m_address= addressLineEdit->text();
    accept();
}


bool netForm::isServer()
{
    return m_server;
}


QString netForm::getAddress()
{
    return m_address;
}
