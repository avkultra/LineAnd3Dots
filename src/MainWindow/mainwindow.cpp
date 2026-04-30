#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../GraphicsViewEx/graphicsviewex.h"
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QStatusBar>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "const.h"

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent)
  , m_bHasData(false)
  , m_startId(-1)
  , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Line solver");
    setMinimumSize(1024, 768);

    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    m_graphView = new CGraphicsViewEx(this);

    QWidget* controlPanel = new QWidget(this);

    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(m_graphView, 1);

    setCentralWidget(central);

    statusBar()->showMessage(tr("tr_Ready"), 5000);

    connect(ui->actionLoadFile, &QAction::triggered, this, &CMainWindow::onLoadFile);
    connect(ui->actionSolve, &QAction::triggered, this, &CMainWindow::onSolveProblem);
    connect(ui->actionZoomIn, &QAction::triggered, this, &CMainWindow::onZoomIn);
    connect(ui->actionZoomOut, &QAction::triggered, this, &CMainWindow::onZoomOut);
    connect(ui->actionResetView, &QAction::triggered, this, &CMainWindow::onResetView);
    connect(ui->actionToggleGrid, &QAction::toggled, this, &CMainWindow::onToggleGrid);

    ui->actionSolve->setEnabled(false);
}

CMainWindow::~CMainWindow()
{
    delete ui;
}

bool CMainWindow::parseJsonFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull())
    {
        return false;
    }

    QJsonObject root = doc.object();

    if (!root.contains("points") || !root.contains("startPoint"))
    {
        return false;
    }

    QJsonArray pointsArray = root["points"].toArray();
    m_vPoints.clear();

    for (int i = 0; i < pointsArray.size(); ++i)
    {
        QJsonObject pointObj = pointsArray[i].toObject();
        double x = pointObj["x"].toDouble();
        double y = pointObj["y"].toDouble();
        int id = pointObj.value("id").toInt(m_vPoints.size());
        m_vPoints.emplace_back(x, y, id);
    }

    m_startId = root["startPoint"].toInt();

    std::sort(m_vPoints.begin(), m_vPoints.end(),
              [](const CPoint& a, const CPoint& b) { return a.m_id < b.m_id; });

    file.close();
    return (m_startId >= 0 && m_startId < (int)m_vPoints.size());
}


bool CMainWindow::parseFile(const QString& filename)
{
    if (filename.endsWith(".json", Qt::CaseInsensitive))
    {
        return parseJsonFile(filename);
    }
    return false;
}

void CMainWindow::displayPoints()
{
    m_graphView->clearUserItems();

    for (size_t i = 0; i < m_vPoints.size(); ++i)
    {
        CPoint p = m_vPoints[i];
        bool isStart = (p.m_id == m_startId);
        m_graphView->drawPoint(p.m_dX, p.m_dY, p.m_id, isStart);
    }

    QString str("tr_PontsAndStart");
    statusBar()->showMessage(str.arg(m_vPoints.size()).arg(m_startId), 3000);
}

double CMainWindow::compAngl(const CPoint& from, const CPoint& to)
{
    return atan2(to.m_dY - from.m_dY, to.m_dX - from.m_dX);
}


// Симулирует вращение прямой по часовой стрелке с переключением осей.
// Параметры:
//   startAngle - начальный угол прямой (в радианах) относительно оси X
// Возвращает:
//   true  - если прямая может последовательно коснуться всех точек
//   false - если обход всех точек невозможен при данном угле
bool CMainWindow::simCiclePath(double startAngle)
{
    // ==================== ИНИЦИАЛИЗАЦИЯ ====================

    // Если множество точек пусто, моделирование невозможно
    if (m_vPoints.empty())
        return false;

    // Вектор отслеживания посещённых точек
    // false - не посещена, true - посещена
    std::vector<bool> visited(m_vPoints.size(), false);

    // Стартовая точка считается посещённой с самого начала
    visited[m_startId] = true;

    // Текущая опорная точка (ось вращения) - начинаем со стартовой
    int currentId = m_startId;

    // Текущее направление прямой (угол, под которым она идёт из текущей точки)
    double currentAngle = startAngle;

    // Количество посещённых точек (пока только стартовая)
    int visitedCount = 1;

    // ==================== ОСНОВНОЙ ЦИКЛ МОДЕЛИРОВАНИЯ ====================

    // Максимум 2*N шагов - гарантирует завершение, даже если процесс зациклится
    // (N = общее количество точек)
    for (size_t step = 0; step < m_vPoints.size() * 2; ++step)
    {
        // Индекс следующей точки, которую зацепит прямая (по часовой стрелке)
        int nextId = -1;

        // Минимальный положительный угловой сдвиг от текущего направления
        // Инициализируем очень большим числом
        double minDelta = dMinDelta;  // = 1e9

        // ==================== ПОИСК БЛИЖАЙШЕЙ ТОЧКИ ПО ЧАСОВОЙ СТРЕЛКЕ ====================

        // Перебираем все точки, чтобы найти ближайшую по часовой стрелке
        for (size_t i = 0; i < m_vPoints.size(); ++i)
        {
            // Пропускаем текущую опорную точку (не можем перейти в саму себя)
            if ((int)i == currentId)
                continue;

            // Вычисляем угол от текущей опорной точки до точки i
            double angle = compAngl(m_vPoints[currentId], m_vPoints[i]);

            // Вычисляем разницу между углом до точки и текущим направлением прямой
            double delta = angle - currentAngle;

            // Нормализуем delta в диапазон [0, 2π)
            // Это даёт нам угол поворота ПО ЧАСОВОЙ СТРЕЛКЕ
            while (delta <= 0)
                delta += 2 * M_PI;
            while (delta >= 2 * M_PI)
                delta -= 2 * M_PI;

            // Если этот поворот меньше наименьшего найденного - запоминаем точку
            // dMinRad = 1e-9 (точность сравнения, исключает погрешности float)
            if (delta < minDelta - dMinRad)
            {
                minDelta = delta;      // Обновляем минимальный угол поворота
                nextId = i;            // Запоминаем индекс ближайшей точки
            }
        }

        // Если следующая точка не найдена (ошибка в алгоритме) - завершаем с неудачей
        if (nextId == -1)
            return false;

        // ==================== ПЕРЕКЛЮЧЕНИЕ ОСИ ВРАЩЕНИЯ ====================

        // Обновляем текущий угол: он становится направлением на только что выбранную точку
        currentAngle = compAngl(m_vPoints[currentId], m_vPoints[nextId]);

        // Переключаем ось вращения на найденную точку
        currentId = nextId;

        // Если эта точка ещё не была посещена - отмечаем её и увеличиваем счётчик
        if (!visited[currentId])
        {
            visited[currentId] = true;   // Отмечаем как посещённую
            ++visitedCount;              // Увеличиваем счётчик посещений
        }

        // ==================== ПРОВЕРКА УСЛОВИЯ УСПЕХА ====================

        // Если посетили все точки - моделирование успешно завершено
        // (не требуется возврат в стартовую точку)
        if (visitedCount == (int)m_vPoints.size())
            return true;
    }

    // Если за отведённое количество шагов не посетили все точки - неудача
    // (произошло зацикливание, и некоторые точки остались недоступными)
    return false;
}

// Находит начальный угол прямой, при котором прямая сможет последовательно
// коснуться всех точек при вращении по часовой стрелке.
// Возвращает:
//   угол в радианах (от 0 до 2pi) - если решение найдено
//   -1e9 (dMinDelta) - если решение не существует
double CMainWindow::findLine()
{
    // ==================== ПРОВЕРКА НАЛИЧИЯ ДАННЫХ ====================

    // Если список точек пуст, возвращаем 0 (угол не имеет значения)
    // Решения нет, но угол не определён
    if (m_vPoints.empty())
        return 0;

    // ==================== ГЕНЕРАЦИЯ УГЛОВ-КАНДИДАТОВ ====================

    // Получаем ссылку на стартовую точку (для удобства и читаемости кода)
    const CPoint& start = m_vPoints[m_startId];

    // Вектор для хранения углов-кандидатов, которые будем тестировать
    std::vector<double> testAngles;

    // ----- 1. Углы, соответствующие направлениям на другие точки -----
    // Это основные кандидаты, так как прямая чаще всего проходит через точки
    for (size_t i =0; i < m_vPoints.size(); ++i)
    {
        const CPoint& p = m_vPoints[i];
        // Пропускаем саму стартовую точку
        // (угол от точки к самой себе не определён)
        if (p.m_id == m_startId)
            continue;

        // Вычисляем угол от стартовой точки до текущей точки p
        double ang = compAngl(start, p);

        // Добавляем сам угол
        testAngles.push_back(ang);

        // Добавляем угол с небольшим положительным смещением (dOffset = 1e-6)
        // Это нужно, чтобы поймать прямые, которые проходят чуть дальше точки
        // (из-за погрешностей вычислений с плавающей точкой)
        testAngles.push_back(ang + dOffset);

        // Добавляем угол с небольшим отрицательным смещением
        // Это покрывает пограничные случаи, когда прямая должна быть чуть ближе
        testAngles.push_back(ang - dOffset);
    }

    // ----- 2. Равномерная сетка углов (страховочный вариант) -----
    // Добавляем 360 равномерно распределённых углов (через 1 градус)
    // Это гарантирует, что мы не пропустим решение, которое не совпадает
    // с направлениями на существующие точки (например, прямая проходит между точками)
    for (int i = 0; i < 360; i++)
    {
        testAngles.push_back(i * M_PI / 180.0);
    }

    // ==================== ОПТИМИЗАЦИЯ ====================

    // Сортируем все углы-кандидаты по возрастанию
    // Это нужно для последующего удаления дубликатов
    std::sort(testAngles.begin(), testAngles.end());

    // Удаляем дубликаты (углы, которые отличаются менее чем на dMinRad = 1e-9)
    // Это оптимизация: не нужно проверять одинаковые углы несколько раз
    // Лямбда-функция сравнивает два угла с заданной точностью
    testAngles.erase(
        std::unique(testAngles.begin(), testAngles.end(),
            [](double a, double b) { return fabs(a - b) < dMinRad; }),
        testAngles.end()
    );

    // ==================== ПОИСК РЕШЕНИЯ ====================

    // Перебираем все углы-кандидаты в порядке возрастания
    // (порядок не важен, но так удобнее для отладки)
    for (double angle : testAngles)
    {
        // Для каждого угла запускаем моделирование процесса вращения
        // simCiclePath симулирует вращение прямой и возвращает true,
        // если прямая смогла последовательно коснуться всех точек
        if (simCiclePath(angle))
        {
            // Если моделирование успешно (прямая прошла через все точки),
            // возвращаем этот угол как найденное решение
            return angle;
        }
    }

    // ==================== РЕШЕНИЕ НЕ НАЙДЕНО ====================

    // Если ни один из кандидатов не подошёл, возвращаем специальное значение
    // -dMinDelta = -1e9 сигнализирует о том, что решение не найдено
    // Отрицательное значение выбрано, так как углы лежат в диапазоне [0, 2π)
    return -dMinDelta;
}

void CMainWindow::onLoadFile()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                   tr("tr_OpenFile"),
                                                    "",
                                                     tr("tr_FileExt"));

    if (filename.isEmpty()) return;

    if (parseFile(filename))
    {
        m_bHasData = true;
        ui->actionSolve->setEnabled(true);
        displayPoints();
        statusBar()->showMessage(tr("tr_SuccessFileOnen") + filename, 3000);
        statusBar()->showMessage(tr("tr_DataLoaded"), 2000);
        QString str(tr("tr_LoadPonts"));
        QMessageBox::information(this, tr("tr_Success"),
                                 str.arg(m_vPoints.size()).arg(m_startId));
    }
    else
    {
        m_bHasData = false;
        ui->actionSolve->setEnabled(false);
        statusBar()->showMessage(tr("tr_Error"), 3000);
        QMessageBox::critical(this, tr("tr_Error"),
                              tr("tr_UnSuccessLoadfile"));
     }
}

void CMainWindow::onSolveProblem()
{
    if (!m_bHasData)
    {
        QMessageBox::warning(this, tr("tr_Warning")
                             , tr("tr_BeforeLoadPoints"));
        return;
    }

    statusBar()->showMessage(tr("tr_FindOfSolver"));
    QApplication::setOverrideCursor(Qt::WaitCursor);

    double solutionAngle = findLine();

    QApplication::restoreOverrideCursor();

    if (solutionAngle > -dMinAng)
    {
        const CPoint& start = m_vPoints[m_startId];
        m_graphView->drawLineThrough(start.m_dX, start.m_dY, solutionAngle, QColor(200, 50, 50));
        statusBar()->showMessage(tr("tr_SuccessSolver"), 3000);

        QMessageBox::information(this, tr("tr_SuccessSolver")
                                , tr("tr_SuccessSolverInfo"));
    }
    else
    {
         statusBar()->showMessage(tr("tr_UnSuccessSolver"), 3000);
        QMessageBox::warning(this, tr("tr_UnSuccessSolver"),
                             tr("tr_UnSuccessSolverInfo"));
    }
}

void CMainWindow::onToggleGrid(bool bShow)
{
    m_graphView->drawGrid(bShow);
}

void CMainWindow::onZoomIn()
{
    m_graphView->scale(1.2, 1.2);
}

void CMainWindow::onZoomOut()
{
    m_graphView->scale(0.8, 0.8);
}

void CMainWindow::onResetView()
{
    m_graphView->resetTransform();
    m_graphView->centerOn(0, 0);
}
