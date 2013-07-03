
#include "options_dialog.h"

#include <QFileDialog>

namespace swganh {

    OptionsDialog::OptionsDialog(QWidget* parent, QString project_directory)
        : QDialog(parent)
    {
        setupUi(this);

        projectDirectory->setText(project_directory);


        connect(chooseProjectDirectory, SIGNAL(clicked()), this, SLOT(slotChooseProjectDirectory()));
        connect((QObject *) buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(slotDefaults()));
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void OptionsDialog::slotChooseProjectDirectory()
    {
        QString s = QFileDialog::getExistingDirectory(
            this,
            tr("Choose the project directory"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (s.isEmpty())
        {
            return;
        }

        projectDirectory->setText(s);
    }

    void OptionsDialog::slotDefaults()
    {
        projectDirectory->setText("");
    }

}
