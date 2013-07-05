
#include <QtWidgets>

#include <boost/filesystem.hpp>

#include "main_window.h"
#include "options_dialog.h"
#include "project_manager.h"

namespace swganh {

    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
    {
        setupUi(this);

        project_directory_ = "";
        project_manager_ = new ProjectManager(this, treeFiles);

        loadSettings();

        connect(actionOptions, SIGNAL(triggered()), this, SLOT(slotOptions()));
    }

    bool MainWindow::openProject(QString project_directory)
    {
        project_manager_->openProject(project_directory);

        return true;
    }

    void MainWindow::closeProject()
    {
        project_manager_->closeProject();
    }

    void MainWindow::closeEvent(QCloseEvent *event)
    {
        saveSettings();
        event->accept();
    }

    void MainWindow::slotOptions()
    {
        OptionsDialog* options = new OptionsDialog(this, project_directory_);

        if (options->exec() == QDialog::Accepted)
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            auto project_dir = options->projectDirectory->text();

            if (project_dir.compare(getProjectDirectory()) != 0) {
                closeProject();
                
                setProjectDirectory(project_dir);

                if (boost::filesystem::is_directory(project_dir.toStdString()))
                {
                    openProject(project_dir);
                }
            }

            QApplication::restoreOverrideCursor();
        }
    }

    void MainWindow::setProjectDirectory(QString dir)
    {
        project_directory_ = dir;
    }

    void MainWindow::loadSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ANH Studios", "SWGEd");

        settings.beginGroup("main");
        setProjectDirectory(settings.value("project_directory", QString("")).toString());
        settings.endGroup();
    }

    void MainWindow::saveSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ANH Studios", "SWGEd");

        settings.beginGroup("main");
        settings.setValue("project_directory", getProjectDirectory());
        settings.endGroup();
    }

}
