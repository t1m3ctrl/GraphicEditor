#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QElapsedTimer>

#include <functionmodel.h>
#include <simpleparser.h>
#include <rangedialog.h>
#include <helpdialog.h>
#include <loginwindow.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FunctionModel *model;   // Модель функций
    QModelIndex contextMenuIndex;
    QString currentFile;

    QLabel *sessionTimerLabel;   // Отображение текущей сессии
    QTimer *sessionTimer;        // Таймер для обновления времени
    QElapsedTimer sessionElapsed; // Хранение времени текущей сессии

    void updateSessionTime();
    bool validateFunction(const QString &function);

    void saveSettings();
    void loadSettings();

private slots:
    void on_UpdatePlot();
    void on_addFunction_clicked();
    void on_removeFunction_clicked();
    void on_CustomContextMenuRequested(const QPoint &pos);
    void on_ChangeLineColor();
    void on_ChangeLineStyle();
    void on_ChangeLineThickness();
    void on_range_triggered();
    void on_color_triggered();
    void on_style_triggered();
    void on_thickness_triggered();
    void on_clear_triggered();
    void on_exit_triggered();
    void on_saveFile_triggered();
    void on_openFile_triggered();
    void on_newFile_triggered();
    void on_saveFileAs_triggered();
    void saveToFile(const QString &fileName);
    void on_help_2_triggered();
    void onUpdateTimer();
};
#endif // MAINWINDOW_H
