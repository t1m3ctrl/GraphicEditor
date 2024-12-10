#ifndef RANGEDIALOG_H
#define RANGEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class RangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RangeDialog(QWidget *parent = nullptr);

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

private:
    QLineEdit *xMinEdit;
    QLineEdit *xMaxEdit;
    QLineEdit *yMinEdit;
    QLineEdit *yMaxEdit;
};

#endif // RANGEDIALOG_H
