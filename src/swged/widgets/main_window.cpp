
#include <QtWidgets>

#include "main_window.h"
#include "options_dialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    project_directory_ = QDir::homePath();

    connect(actionOptions, SIGNAL(triggered()), this, SLOT(slotOptions()));    
}

void MainWindow::slotOptions()
{
    OptionsDialog* options = new OptionsDialog(this);

    if (options->exec() == QDialog::Accepted)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        
        auto project_dir = options->projectDirectory->text();

        setProjectDirectory(project_dir);
    }
}

void MainWindow::setProjectDirectory(QString dir)
{
    project_directory_ = dir;
}
