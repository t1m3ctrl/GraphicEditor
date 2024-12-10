#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QFile>

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private:
    QTextBrowser *textBrowser;

    void loadHelpContent();
};

#endif // HELPDIALOG_H
