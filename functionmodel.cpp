#include "functionmodel.h"

FunctionModel::FunctionModel(QObject *parent)
    : QAbstractListModel(parent) {}

int FunctionModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return functions.size();
}

QVariant FunctionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= functions.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return functions.at(index.row()).expression;
    }

    return QVariant();
}

void FunctionModel::addFunction(const QString &function) {
    beginInsertRows(QModelIndex(), functions.size(), functions.size());
    functions.append({function, defaultColor, defaultStyle, defaultThickness}); // Цвет по умолчанию — черный, стиль — сплошная линия
    endInsertRows();
}

void FunctionModel::addFunction(const QString &function, QColor color, Qt::PenStyle style, int thickness) {
    beginInsertRows(QModelIndex(), functions.size(), functions.size());
    functions.append({function, color, style, thickness});
    endInsertRows();
}

void FunctionModel::removeFunction(int row) {
    if (row < 0 || row >= functions.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    functions.removeAt(row);
    endRemoveRows();
}

FunctionProperties FunctionModel::getFunctionProperties(int row) const {
    if (row >= 0 && row < functions.size()) {
        return functions.at(row);
    }
    return {};
}

void FunctionModel::updateFunctionProperties(int row, const QColor &color, Qt::PenStyle style, int thickness) {
    if (row >= 0 && row < functions.size()) {
        functions[row].color = color;
        functions[row].style = style;
        functions[row].thickness = thickness;
        emit dataChanged(index(row), index(row));
    }
}

void FunctionModel::clear() {
    beginResetModel();
    functions.clear();
    endResetModel();
}

void FunctionModel::saveToDatabase(const QString &username) {
    QSqlQuery query;

    // Удаляем все функции текущего пользователя перед сохранением новых
    query.prepare("DELETE FROM functions WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        qDebug() << "Ошибка удаления функций из БД:" << query.lastError().text();
        return;
    }

    // Сохраняем все функции для текущего пользователя
    for (const FunctionProperties &props : functions) {
        query.prepare("INSERT INTO functions (username, expression, color, style, thickness) "
                      "VALUES (:username, :expression, :color, :style, :thickness)");
        query.bindValue(":username", username);
        query.bindValue(":expression", props.expression);
        query.bindValue(":color", props.color.name());  // Преобразуем QColor в строку HEX (#RRGGBB)
        query.bindValue(":style", static_cast<int>(props.style));  // Преобразуем стиль в int
        query.bindValue(":thickness", props.thickness);  // Толщина линии

        if (!query.exec()) {
            qDebug() << "Ошибка при сохранении функции:" << query.lastError().text();
        }
    }
}

void FunctionModel::loadFromDatabase(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT expression, color, style, thickness FROM functions WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Ошибка загрузки функций из БД:" << query.lastError().text();
        return;
    }

    clear(); // Очищаем модель перед загрузкой новых функций

    while (query.next()) {
        QString expression = query.value(0).toString();
        QColor color(query.value(1).toString());
        Qt::PenStyle style = static_cast<Qt::PenStyle>(query.value(2).toInt());
        int thickness = query.value(3).toInt();

        addFunction(expression, color, style, thickness);
    }
}
