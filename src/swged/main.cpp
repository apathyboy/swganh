
#include <stdio.h>
#include <stdlib.h>

#include <QApplication>
#include <QFile>
#include <QSplashScreen>

#include "widgets/main_window.h"

using swganh::MainWindow;

QSplashScreen* splash = 0;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   FILE * pFile;
   pFile = fopen ("myfile.txt","a");
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(pFile, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(pFile, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(pFile, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(pFile, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
    fclose(pFile);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput); //install : set the callback
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
