
#pragma once

#include "ui_main_window.h"

namespace swganh {

    class ProjectManager;

    class MainWindow : public QMainWindow, private Ui::MainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = 0);

        bool openProject(QString project_directory);
        QString getProjectDirectory() const { return project_directory_; }

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        QString project_directory_;
        ProjectManager* project_manager_;

        void setProjectDirectory(QString dir);

        void loadSettings();
        void saveSettings();

        private slots:
        void slotOptions();
    };

}
