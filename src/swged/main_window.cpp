
#include <QtWidgets>

#include "main_window.h"

MainWindow::MainWindow()
{
    mdi_area_ = new QMdiArea;
    setCentralWidget(mdi_area_);

    createActions();
    createMenus();
    createStatusBar();
    createDockWindows();

    setWindowTitle(tr("SWGEd"));

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Dock Widgets"),
            tr("The <b>SWGEd</b> application is a tool for creating "
               "and managing Star Wars Galaxies game content."));
}

void MainWindow::createActions()
{
    quit_action_ = new QAction(tr("&Quit"), this);
    quit_action_->setShortcuts(QKeySequence::Quit);
    quit_action_->setStatusTip(tr("Quit the application"));
    connect(quit_action_, SIGNAL(triggered()), this, SLOT(close()));

    about_action_ = new QAction(tr("&About"), this);
    about_action_->setStatusTip(tr("Show information about SWGEd"));
    connect(about_action_, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    file_menu_ = menuBar()->addMenu(tr("&File"));
    file_menu_->addSeparator();
    file_menu_->addAction(quit_action_);

    edit_menu_ = menuBar()->addMenu(tr("&Edit"));

    view_menu_ = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    help_menu_ = menuBar()->addMenu(tr("&Help"));
    help_menu_->addAction(about_action_);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Resources"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    resource_tree_ = new QTreeWidget(dock);

    dock->setWidget(resource_tree_);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    view_menu_->addAction(dock->toggleViewAction());
}
