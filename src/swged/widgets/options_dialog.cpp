
#include "options_dialog.h"

#include "qfiledialog.h"

OptionsDialog::OptionsDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(chooseProjectDirectory, SIGNAL(clicked()), this, SLOT(slotChooseProjectDirectory()));
}

void OptionsDialog::slotChooseProjectDirectory()
{
    QString s = QFileDialog::getExistingDirectory(
        this,
        tr("Choose the project directory"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(s.isEmpty())
    {
        return;
    }

    projectDirectory->setText(s);
}
