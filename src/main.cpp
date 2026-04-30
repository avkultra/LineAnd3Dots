#include "MainWindow\mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "LineAnd3Dots_" + QLocale(locale).name();
        if (translator.load("translations/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    CMainWindow w;
    w.show();
    return a.exec();
}
