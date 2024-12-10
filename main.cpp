#include "mainwindow.h"
#include "loginwindow.h"

#include <QApplication>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("graphic");
    db.setUserName("postgres");
    db.setPassword("password");

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Error", "Cannot open database.");
        return -1;
    }

    QString filePath = ":/resources/style.qss";  // В случае использования ресурсов
    QFile file(filePath);
    // QFile file("style.qss");  // Если файл находится в каталоге проекта
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&file);
        QString style = ts.readAll();
        a.setStyleSheet(style);
    }

    LoginWindow loginWindow;

    if (loginWindow.exec() == QDialog::Accepted) {
        // Если авторизация прошла успешно, открываем MainWindow
        MainWindow mainWindow;
        mainWindow.show();
        return a.exec();
    }


    return 0;
}
