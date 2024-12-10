#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
{
    this->setWindowTitle("Login / Register");
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


bool LoginWindow::authenticateUser(const QString &username, const QString &password)
{
    // Хэшируем пароль (можно использовать более безопасные алгоритмы, такие как bcrypt)
    QString hashedPassword = QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (query.exec() && query.next()) {
        return true; // Пользователь найден
    }
    return false;
}

bool LoginWindow::registerUser(const QString &username, const QString &password)
{
    QString hashedPassword = QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

void LoginWindow::on_loginButton_clicked()
{
    QString username = ui->usernameInput->text();
    QString password = ui->passwordInput->text();

    if (authenticateUser(username, password)) {
        // Пользователь успешно вошел, сохранение в QSettings
        QSettings settings("MyCompany", "MyApp");
        settings.setValue("user/username", username);

        accept();  // Закрыть окно и перейти к основному окну
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}


void LoginWindow::on_registerButton_clicked()
{
    QString username = ui->usernameInput->text();
    QString password = ui->passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Registration Failed", "User registration failed. Fields are empty");
        return;
    }
    if (registerUser(username, password)) {
        QMessageBox::information(this, "Registration", "User registered successfully.");
    } else {
        QMessageBox::warning(this, "Registration Failed", "User registration failed.");
    }
}

