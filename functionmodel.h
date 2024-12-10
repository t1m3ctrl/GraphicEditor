#ifndef FUNCTIONMODEL_H
#define FUNCTIONMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QColor>
#include <QSqlQuery>
#include <QSqlError>

struct FunctionProperties {
    QString expression; // Уравнение функции
    QColor color;       // Цвет линии
    Qt::PenStyle style; // Стиль линии
    int thickness;
};

class FunctionModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit FunctionModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addFunction(const QString &function);
    void addFunction(const QString &function, QColor color, Qt::PenStyle style, int thickness);
    void removeFunction(int row);
    FunctionProperties getFunctionProperties(int row) const;
    void updateFunctionProperties(int row, const QColor &color, Qt::PenStyle style, int thickness);

    void setDefaultColor(const QColor &color) { defaultColor = color; }
    void setDefaultStyle(Qt::PenStyle style) { defaultStyle = style; }
    void setDefaultThickness(int thickness) { defaultThickness = thickness; }

    QColor getDefaultColor() { return defaultColor; }
    Qt::PenStyle getDefaultStyle() { return defaultStyle; }
    int getDefaultThickness() { return defaultThickness; }

    void saveToDatabase(const QString &username);
    void loadFromDatabase(const QString &username);

    void clear();

private:
    QVector<FunctionProperties> functions; // Список функций и их свойств

    QColor defaultColor = Qt::black;  // Стандартный цвет
    Qt::PenStyle defaultStyle = Qt::SolidLine; // Стандартный стиль
    int defaultThickness = 1;  // Стандартная толщина
};

#endif // FUNCTIONMODEL_H
