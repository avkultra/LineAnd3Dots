#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include "../Point/point.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CGraphicsViewEx;
class QTimer;
class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CMainWindow(QWidget* parent = nullptr);
    ~CMainWindow();
private slots:
    void onLoadFile();
    void onSolveProblem();
    void onToggleGrid(bool show);
    void onZoomIn();
    void onZoomOut();
    void onResetView();

private:
    CGraphicsViewEx* m_graphView;
    QLabel* m_statusLabel;
    QLabel* m_infoLabel;

    std::vector<CPoint> m_vPoints;
    bool m_bHasData;
    int m_startId;

    bool parseJsonFile(const QString& filename);
    bool parseFile(const QString& filename);
    void displayPoints();
    double compAngl(const CPoint& from, const CPoint& to);
    bool simCiclePath(double startAngle);
    double findLine();
private:
     Ui::MainWindow* ui;

private slots:
    void onAnimationTimer();
    void onStartAnimation();

private:
    QTimer* m_animationTimer;
    double m_currentAngle;
    int m_currentPivotId;
    int m_animationStep;
    std::vector<int> m_animationOrder;
    bool m_isAnimating;
};

#endif // MAINWINDOW_H
