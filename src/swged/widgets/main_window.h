
#pragma once

#include <QFutureWatcher>
#include <QProgressBar>

#include "ui_main_window.h"

namespace swganh {

    class ProjectManager;

    class MainWindow : public QMainWindow, public Ui::MainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = 0);

        QFuture<void> openProject();

        void showStatusProgress(bool visible);

    signals:
        void options();

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        ProjectManager* project_manager_;

        QFutureWatcher<void> future_watcher_;
        QProgressBar* status_progress_bar_;
        QString project_directory_;

        void loadSettings();
        void saveSettings();

    private slots:
        void slotAbout();
        void slotOptions();
        void slotProjectLoadFinished();
        void closeDocumentTab(int index);
    };

}
