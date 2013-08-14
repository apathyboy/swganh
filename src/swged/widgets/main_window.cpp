
#include "main_window.h"

#include "ui_about_dialog.h"

#include <QtConcurrent>
#include <QtWidgets>

#include <boost/filesystem.hpp>

#include "extract_dialog.h"
#include "options_dialog.h"
#include "project_manager.h"

#include "version.h"

namespace swganh {

    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
    {
        setupUi(this);

        project_manager_ = new ProjectManager(this, treeFiles);

        loadSettings();

        connect(actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
        connect(actionOptions, SIGNAL(triggered()), this, SLOT(slotOptions()));
        connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
        connect(this, SIGNAL(options()), this, SLOT(slotOptions()));
        connect(documentsTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeDocumentTab(int)));

        status_progress_bar_ = new QProgressBar(this);
        status_progress_bar_->setVisible(false);
        status_progress_bar_->setMinimum(0);
        status_progress_bar_->setMaximum(0);

        statusbar->addPermanentWidget(status_progress_bar_);

        connect(&future_watcher_, SIGNAL(finished()), this, SLOT(slotProjectLoadFinished()));

    }

    void MainWindow::closeEvent(QCloseEvent *event)
    {
        saveSettings();
        event->accept();
    }

    void MainWindow::slotAbout()
    {
        QDialog about;
        Ui::AboutDialog ui;
        ui.setupUi(&about);
        ui.version->setText(tr("Version %1").arg(SWGANH_VERSION));
        about.exec();
    }

    void MainWindow::slotOptions()
    {
        OptionsDialog* options = new OptionsDialog(this, project_manager_->getProjectDirectory());

        if (options->exec() == QDialog::Accepted)
        {
            project_directory_ = options->projectDirectory->text();

            auto future = openProject();
            future_watcher_.setFuture(future);

            statusbar->showMessage(tr("Opening project..."));
            showStatusProgress(true);
        }

        options->deleteLater();
    }

    QFuture<void> MainWindow::openProject()
    {
        return QtConcurrent::run([this]() {
            if (!project_manager_->openProject(project_directory_))
            {
                emit options();
            }
        });
    }

    void MainWindow::showStatusProgress(bool visible)
    {
        status_progress_bar_->setVisible(visible);
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

    void MainWindow::slotProjectLoadFinished()
    {
        statusbar->clearMessage();
        showStatusProgress(false);
    }

    void MainWindow::closeDocumentTab(int index)
    {
        if (index == -1)
        {
            return;
        }

        QWidget* tabItem = documentsTabWidget->widget(index);
        documentsTabWidget->removeTab(index);
        delete tabItem;
    }
}
