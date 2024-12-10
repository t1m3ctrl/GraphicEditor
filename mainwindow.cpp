#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new FunctionModel(this))
    , sessionTimerLabel(new QLabel(this))
    , sessionTimer(new QTimer(this))
{
    ui->setupUi(this);

    loadSettings();

    sessionElapsed.start();
    connect(sessionTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimer);
    sessionTimer->start(1000);
    statusBar()->addPermanentWidget(sessionTimerLabel);

    ui->listView->setModel(model);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->setInteraction(QCP::iRangeDrag, true);

    connect(model, &QAbstractListModel::dataChanged, this, &MainWindow::on_UpdatePlot);
    connect(model, &QAbstractListModel::rowsInserted, this, &MainWindow::on_UpdatePlot);
    connect(model, &QAbstractListModel::rowsRemoved, this, &MainWindow::on_UpdatePlot);

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, &QListView::customContextMenuRequested, this, &MainWindow::on_CustomContextMenuRequested);
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(on_UpdatePlot()));
    on_UpdatePlot();
}

MainWindow::~MainWindow()
{
    saveSettings();

    delete ui;
}

void MainWindow::on_addFunction_clicked()
{
    QString function = ui->enterFunction->text().trimmed();

    if (!validateFunction(function)) {
        return;
    }

    if (!function.isEmpty()) {
        model->addFunction(function);
        ui->enterFunction->clear();
    }
}

void MainWindow::on_removeFunction_clicked()
{
    QModelIndex currentIndex = ui->listView->currentIndex();
    if (currentIndex.isValid()) {
        model->removeFunction(currentIndex.row());
    }
}

void MainWindow::on_UpdatePlot()
{
    ui->plot->clearGraphs();

    double xMin = ui->plot->xAxis->range().lower;
    double xMax = ui->plot->xAxis->range().upper;

    int points = static_cast<int>((xMax - xMin) * 10);  // Умножаем на коэффициент для точности
    points = std::min(points, 20000);  // Ограничиваем верхний предел 20000 точками
    points = std::max(points, 100);    // Ограничиваем нижний предел 100 точками

    double step = (xMax - xMin) / points;

    QVector<double> x(points + 1), y(points + 1);

    for (int i = 0; i < model->rowCount(); i++) {
        FunctionProperties props = model->getFunctionProperties(i);
        QString function = props.expression;

        for (int j = 0; j <= points; ++j) {
            x[j] = xMin + j * step;
            y[j] = SimpleParser::evaluate(function, x[j]);

            if (std::isnan(y[i]) || std::isinf(y[i])) {
                y[i] = 0;  // или какое-то другое значение по умолчанию
            }
        }

        ui->plot->addGraph();
        ui->plot->graph(i)->setData(x, y);
        ui->plot->graph(i)->setPen(QPen(props.color, props.thickness, props.style));
    }

    ui->plot->replot();
}

bool MainWindow::validateFunction(const QString &expression) {
    if (expression.isEmpty()) {
        // Ошибка ввода: пустая строка
        QMessageBox::critical(nullptr, "Ошибка", "Пустая строка.");
        return false;
    }

    // Убираем все пробелы в начале и в конце строки
    QString trimmedExpr = expression.trimmed();

    // Проверка на наличие только разрешенных символов (цифры, буквы, операторы, скобки)
    // QRegularExpression regex("^[0-9a-zA-Z\\+\\-\\*/^()\\.x]+$");
    // if (!regex.match(trimmedExpr).hasMatch()) {
    //     QMessageBox::critical(nullptr, "Ошибка", "Функция содержит недопустимые символы.");
    //     return false;
    // }

    // Проверка на наличие нескольких подряд идущих знаков операций
    QRegularExpression operatorRegex("[\\+\\-\\*/^]{2,}");
    if (operatorRegex.match(trimmedExpr).hasMatch()) {
        QMessageBox::critical(nullptr, "Ошибка", "Функция содержит подряд идущие операторы.");
        return false;
    }

    // Использование tinyexpr для проверки математического выражения с переменной x
    std::string exprStd = trimmedExpr.toStdString();
    const char *expr = exprStd.c_str();
    te_variable variables[] = {
        { "x", nullptr , 0, nullptr} // Переменная x
    };
    // В данном случае мы не используем значения для переменной x, просто проверяем синтаксис
    te_expr *parsed_expr = te_compile(expr, variables, sizeof(variables) / sizeof(variables[0]), nullptr);

    if (parsed_expr == nullptr) {
        // Если выражение не может быть разобрано, оно некорректно
        QMessageBox::critical(nullptr, "Ошибка", "Некорректное математическое выражение.");
        return false;
    }

    // Освобождаем ресурсы, если выражение валидное
    te_free(parsed_expr);

    return true;
}


void MainWindow::on_CustomContextMenuRequested(const QPoint &pos)
{
    contextMenuIndex = ui->listView->indexAt(pos);
    if (!contextMenuIndex.isValid()) return;

    QMenu contextMenu(this);
    contextMenu.addAction("Изменить цвет линии", this, &MainWindow::on_ChangeLineColor);
    contextMenu.addAction("Изменить стиль линии", this, &MainWindow::on_ChangeLineStyle);
    contextMenu.addAction("Изменить толщину линии", this, &MainWindow::on_ChangeLineThickness);

    contextMenu.exec(ui->listView->mapToGlobal(pos));
}

void MainWindow::on_ChangeLineColor()
{
    if (!contextMenuIndex.isValid()) return;

    QColor color = QColorDialog::getColor(Qt::black, this, "Выберите цвет линии");
    if (color.isValid()) {
        int row = contextMenuIndex.row();
        FunctionProperties props = model->getFunctionProperties(row);
        model->updateFunctionProperties(row, color, props.style, props.thickness);
    }
}

void MainWindow::on_ChangeLineStyle()
{
    if (!contextMenuIndex.isValid()) return;

    QStringList styles = {"Сплошная", "Штриховая", "Точечная", "Штрих-точка"};
    bool ok;
    QString style = QInputDialog::getItem(this, "Выберите стиль линии", "Стиль", styles, 0, false, &ok);
    if (!ok) return;

    Qt::PenStyle penStyle = Qt::SolidLine;
    if (style == "Штриховая") penStyle = Qt::DashLine;
    else if (style == "Точечная") penStyle = Qt::DotLine;
    else if (style == "Штрих-точка") penStyle = Qt::DashDotLine;

    int row = contextMenuIndex.row();
    FunctionProperties props = model->getFunctionProperties(row);
    model->updateFunctionProperties(row, props.color, penStyle, props.thickness);
}

void MainWindow::on_ChangeLineThickness()
{
    if (!contextMenuIndex.isValid()) return;

    bool ok;
    QStringList thicknesses = {"1", "2", "3", "4", "5"};
    int thickness = QInputDialog::getItem(this, "Измените толщину линии", "Толщина", thicknesses, 0, false, &ok).toInt();
    if (!ok) return;

    int row = contextMenuIndex.row();
    FunctionProperties props = model->getFunctionProperties(row);
    model->updateFunctionProperties(row, props.color, props.style, thickness);
}


void MainWindow::on_range_triggered()
{
    RangeDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем значения из диалога
        double xMin = dialog.xMin();
        double xMax = dialog.xMax();
        double yMin = dialog.yMin();
        double yMax = dialog.yMax();

        // Устанавливаем новые диапазоны
        ui->plot->xAxis->setRange(xMin, xMax);
        ui->plot->yAxis->setRange(yMin, yMax);

        // Обновляем график с новыми диапазонами
        on_UpdatePlot();
    }
}

void MainWindow::on_color_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Выберите цвет по умолчанию");
    if (color.isValid()) {
        model->setDefaultColor(color);  // Передаем новое значение в модель
    }
}


void MainWindow::on_style_triggered()
{
    QStringList styles = {"Сплошная", "Штриховая", "Точечная", "Штрих-точка"};
    bool ok;
    QString style = QInputDialog::getItem(this, "Выберите стиль линии", "Стиль", styles, 0, false, &ok);
    if (ok) {
        Qt::PenStyle penStyle;
        if (style == "Штриховая") penStyle = Qt::DashLine;
        else if (style == "Точечная") penStyle = Qt::DotLine;
        else if (style == "Штрих-точка") penStyle = Qt::DashDotLine;
        else penStyle = Qt::SolidLine;
        model->setDefaultStyle(penStyle);  // Передаем новое значение в модель
    }
}


void MainWindow::on_thickness_triggered()
{
    bool ok;
    QStringList thicknesses = {"1", "2", "3", "4", "5"};
    int thickness = QInputDialog::getItem(this, "Измените толщину линии", "Толщина", thicknesses, 0, false, &ok).toInt();
    if (ok) {
        model->setDefaultThickness(thickness);  // Передаем новое значение в модель
    }
}


void MainWindow::on_clear_triggered()
{
    model->clear();
    ui->plot->clearGraphs();
    ui->plot->replot();
}

void MainWindow::on_newFile_triggered()
{
    model->clear();
    ui->plot->clearGraphs();
    ui->plot->replot();

    currentFile.clear(); // Очистка текущего файла
}

void MainWindow::on_openFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Function Files (*.funcplot)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл!");
        return;
    }

    QTextStream in(&file);
    model->clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(';');
        if (parts.size() == 4) {
            QString expression = parts[0];
            QColor color(parts[1]);
            Qt::PenStyle style = static_cast<Qt::PenStyle>(parts[2].toInt());
            int thickness = parts[3].toInt();
            model->addFunction(expression, color, style, thickness);
        }
    }

    file.close();
    currentFile = fileName; // Установка текущего файла
    on_UpdatePlot();
}

void MainWindow::on_saveFile_triggered()
{
    if (currentFile.isEmpty()) {
        on_saveFileAs_triggered();
        return;
    }

    saveToFile(currentFile);
}

void MainWindow::on_saveFileAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл как", "", "Function Files (*.funcplot)");
    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".funcplot")) {
        fileName += ".funcplot"; // Добавление расширения, если отсутствует
    }

    saveToFile(fileName);
    currentFile = fileName;
}

void MainWindow::on_exit_triggered()
{
    saveSettings();
    this->close();

    // Переход к LoginWindow
    LoginWindow loginWindow;
    if (loginWindow.exec() == QDialog::Accepted) {
        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
    }
}

void MainWindow::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл!");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < model->rowCount(); ++i) {
        FunctionProperties props = model->getFunctionProperties(i);
        out << props.expression << ';'
            << props.color.name() << ';'
            << static_cast<int>(props.style) << ';'
            << props.thickness << '\n';
    }

    file.close();
}

void MainWindow::on_help_2_triggered()
{
    HelpDialog helpDialog(this);
    helpDialog.exec();
}

void MainWindow::loadSettings() {
    QString username = QSettings("MyCompany", "MyApp").value("user/username").toString();
    QSettings settings("MyCompany", "MyApp_" + username);

    // Загрузка настроек окна
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());

    // Загрузка настроек графика
    QColor defaultColor = settings.value("plot/defaultColor", QColor(Qt::black)).value<QColor>();
    Qt::PenStyle defaultStyle = static_cast<Qt::PenStyle>(settings.value("plot/defaultStyle", int(Qt::SolidLine)).toInt());
    int defaultThickness = settings.value("plot/defaultThickness", 1).toInt();

    double xMin = settings.value("plot/xMin", ui->plot->xAxis->range().lower).toDouble();
    double xMax = settings.value("plot/xMax", ui->plot->xAxis->range().upper).toDouble();
    double yMin = settings.value("plot/yMin", ui->plot->yAxis->range().lower).toDouble();
    double yMax = settings.value("plot/yMax", ui->plot->yAxis->range().upper).toDouble();

    ui->plot->xAxis->setRange(xMin, xMax);
    ui->plot->yAxis->setRange(yMin, yMax);

    model->setDefaultColor(defaultColor);
    model->setDefaultStyle(defaultStyle);
    model->setDefaultThickness(defaultThickness);
    model->loadFromDatabase(username);
}

void MainWindow::saveSettings() {
    QString username = QSettings("MyCompany", "MyApp").value("user/username").toString();
    QSettings settings("MyCompany", "MyApp_" + username);

    model->saveToDatabase(username);

    // Сохраняем настройки окна
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());

    // Сохраняем настройки графика
    settings.setValue("plot/defaultColor", model->getDefaultColor());
    settings.setValue("plot/defaultStyle", static_cast<int>(model->getDefaultStyle()));
    settings.setValue("plot/defaultThickness", model->getDefaultThickness());
    settings.setValue("plot/xMin", ui->plot->xAxis->range().lower);
    settings.setValue("plot/xMax", ui->plot->xAxis->range().upper);
    settings.setValue("plot/yMin", ui->plot->yAxis->range().lower);
    settings.setValue("plot/yMax", ui->plot->yAxis->range().upper);
}

void MainWindow::onUpdateTimer() {
    updateSessionTime();
}

void MainWindow::updateSessionTime() {
    qint64 sessionSeconds = sessionElapsed.elapsed() / 1000; // Время текущей сессии в секундах
    QTime sessionTime = QTime(0, 0).addSecs(sessionSeconds);

    sessionTimerLabel->setText("Время текущей сессии: " + sessionTime.toString("hh:mm:ss"));
}



