
#include <QApplication>
#include <QFile>
#include <QSplashScreen>

#include "widgets/main_window.h"

using swganh::MainWindow;

QSplashScreen* splash = 0;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("ANH Studios");
    app.setApplicationName("SWGEd");

    QSplashScreen splash(QPixmap(":/images/splash_logo.jpg"));
    splash.setFont( QFont("Helvetica", 10) );
    splash.show();

    splash.showMessage("Loading project", Qt::AlignBottom | Qt::AlignLeft, Qt::white);
    app.processEvents();

    MainWindow mw;

    // Set stylesheet
    QFile file(":/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(styleSheet);

    auto future = mw.openProject();

    while (!future.isFinished())
    {
        app.processEvents();
    }

    mw.show();
    splash.finish(&mw);

    return app.exec();
}
