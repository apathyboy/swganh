
#include <QApplication>
#include <QSplashScreen>

#include "widgets/main_window.h"

QSplashScreen* splash = 0;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("ANH Studios");
    app.setApplicationName("SWGEd");

    QSplashScreen splash(QPixmap(":/images/splash_logo.jpg"));
    splash.setFont( QFont("Helvetica", 10) );
    splash.show();
    
	qApp->processEvents();
    splash.showMessage("Loading project");
    
	qApp->processEvents();
    MainWindow mw;
    mw.show();
    
	qApp->processEvents();
    splash.finish(&mw);

    return app.exec();
}
