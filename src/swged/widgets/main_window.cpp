
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

        project_manager_ = new ProjectManager(this, treeFiles);

        loadSettings();

        connect(actionOptions, SIGNAL(triggered()), this, SLOT(slotOptions()));
    }

    void MainWindow::closeEvent(QCloseEvent *event)
    {
        saveSettings();
        event->accept();
    }

    void MainWindow::slotOptions()
    {
        OptionsDialog* options = new OptionsDialog(this, project_manager_->getProjectDirectory());

        if (options->exec() == QDialog::Accepted)
        {
            auto project_dir = options->projectDirectory->text();
            openProject(project_dir);
        }
    }

    void MainWindow::openProject(QString project_directory)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        if (!project_manager_->openProject(project_directory))
        {
            QApplication::restoreOverrideCursor();
            slotOptions();
        }

        QApplication::restoreOverrideCursor();
    }

    void MainWindow::loadSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ANH Studios", "SWGEd");

        settings.beginGroup("main");
        
        auto project_dir = settings.value("project_directory", QString("")).toString();
        openProject(project_dir);

        settings.endGroup();
    }

    void MainWindow::saveSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ANH Studios", "SWGEd");

        settings.beginGroup("main");
        settings.setValue("project_directory", project_manager_->getProjectDirectory());
        settings.endGroup();
    }

}
