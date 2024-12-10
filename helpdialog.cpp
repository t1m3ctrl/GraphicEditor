#include "HelpDialog.h"

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent), textBrowser(new QTextBrowser(this))
{
    // Настраиваем интерфейс
    setWindowTitle("Справка");
    setMinimumSize(400, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textBrowser);

    // Загружаем содержимое справки
    loadHelpContent();
}

HelpDialog::~HelpDialog()
{
}

void HelpDialog::loadHelpContent()
{
    // Загрузка HTML-файла справки
    QString helpPath = ":/resources/help.html"; // Пути к ресурсам Qt
    QFile helpFile(helpPath);

    if (helpFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = helpFile.readAll();
        textBrowser->setHtml(content);
        helpFile.close();
    } else {
        textBrowser->setText("Справочный файл не найден.");
    }
}
