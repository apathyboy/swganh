
#include <QApplication>
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
    
    splash.showMessage("Loading project", Qt::AlignLeft, Qt::white);
	qApp->processEvents();

    MainWindow mw;

    qApp->processEvents();

    auto project_dir = mw.getProjectDirectory();
    if (!project_dir.isEmpty())
    {
        mw.openProject(project_dir);
    }

    qApp->processEvents();
    mw.show();
    
	qApp->processEvents();
    splash.finish(&mw);

    return app.exec();
}
