
#include "main_window.h"

#include <QtConcurrent>
#include <QtWidgets>

#include <boost/filesystem.hpp>

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
        connect(this, SIGNAL(options()), this, SLOT(slotOptions()));

        proj_load_progress_ = new QProgressDialog(this);

        connect(&future_watcher_, SIGNAL(finished()), this, SLOT(slot_finished()));
        connect(&future_watcher_, SIGNAL(finished()), proj_load_progress_, SLOT(cancel()));

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
            project_directory_ = options->projectDirectory->text();
            openProject();
        }

        options->deleteLater();
    }

    void MainWindow::openProject()
    {
        auto future = QtConcurrent::run([this]() {
            if (!project_manager_->openProject(project_directory_))
            {
                emit options();
            }
        });

        future_watcher_.setFuture(future);

        proj_load_progress_->setMinimum(0);
        proj_load_progress_->setMaximum(0);
        proj_load_progress_->setWindowModality(Qt::WindowModal);
        proj_load_progress_->exec();
    }

    void MainWindow::loadSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ANH Studios", "SWGEd");

        settings.beginGroup("main");
        
        project_directory_ = settings.value("project_directory", QString("")).toString();

        settings.endGroup();
    }

    void MainWindow::saveSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ANH Studios", "SWGEd");

        settings.beginGroup("main");
        settings.setValue("project_directory", project_directory_);
        settings.endGroup();
    }

}
