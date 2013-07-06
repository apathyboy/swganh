
#include "extract_dialog.h"

#include <QFileDialog>

namespace swganh {

    ExtractDialog::ExtractDialog(QWidget* parent, QString default_extract_dir)
        : QDialog(parent)
    {
        setupUi(this);

        extractDir->setText(default_extract_dir);

        connect(chooseExtractDir, SIGNAL(clicked()), this, SLOT(slotChooseExtractDir()));
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    }

    void ExtractDialog::slotChooseExtractDir()
    {
        QString s = QFileDialog::getExistingDirectory(
            this,
            tr("Choose directory to extract files to"),
            extractDir->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (s.isEmpty()) return;

        extractDir->setText(s);
    }

}
