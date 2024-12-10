#include "rangedialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleValidator>

RangeDialog::RangeDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Минимальное значение для оси X:"));
    xMinEdit = new QLineEdit(this);
    xMinEdit->setValidator(new QDoubleValidator(-1000.0, 1000.0, 2, this));
    layout->addWidget(xMinEdit);

    layout->addWidget(new QLabel("Максимальное значение для оси X:"));
    xMaxEdit = new QLineEdit(this);
    xMaxEdit->setValidator(new QDoubleValidator(-1000.0, 1000.0, 2, this));
    layout->addWidget(xMaxEdit);

    layout->addWidget(new QLabel("Минимальное значение для оси Y:"));
    yMinEdit = new QLineEdit(this);
    yMinEdit->setValidator(new QDoubleValidator(-1000.0, 1000.0, 2, this));
    layout->addWidget(yMinEdit);

    layout->addWidget(new QLabel("Максимальное значение для оси Y:"));
    yMaxEdit = new QLineEdit(this);
    yMaxEdit->setValidator(new QDoubleValidator(-1000.0, 1000.0, 2, this));
    layout->addWidget(yMaxEdit);

    QPushButton *okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &RangeDialog::accept);
    layout->addWidget(okButton);

    setLayout(layout);
}

double RangeDialog::xMin() const
{
    return xMinEdit->text().toDouble();
}

double RangeDialog::xMax() const
{
    return xMaxEdit->text().toDouble();
}

double RangeDialog::yMin() const
{
    return yMinEdit->text().toDouble();
}

double RangeDialog::yMax() const
{
    return yMaxEdit->text().toDouble();
}
