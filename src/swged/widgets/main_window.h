
#pragma once

#include <QFutureWatcher>
#include <QProgressDialog>

#include "ui_main_window.h"

namespace swganh {

    class ProjectManager;

    class MainWindow : public QMainWindow, private Ui::MainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = 0);

        void openProject();

    signals:
        void options();

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        ProjectManager* project_manager_;

        QFutureWatcher<void> future_watcher_;
        QProgressDialog* proj_load_progress_;
        QString project_directory_;

        void loadSettings();
        void saveSettings();

    private slots:
        void slotOptions();
    };

}
