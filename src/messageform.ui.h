void messageForm::onOk()
{
    m_message=messageEdit->text();
    accept();
}

QString messageForm::getMessage()
{
    return m_message;
}
