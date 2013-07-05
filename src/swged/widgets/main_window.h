
#pragma once

#include "ui_main_window.h"

namespace swganh {

    class ProjectManager;

    class MainWindow : public QMainWindow, private Ui::MainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = 0);
                
    protected:
        void closeEvent(QCloseEvent *event);

    private:
        ProjectManager* project_manager_;

        void loadSettings();
        void saveSettings();

    private slots:
        void slotOptions();
    };

}
