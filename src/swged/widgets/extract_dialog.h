
#pragma once

#include "ui_extract_dialog.h"

namespace swganh {

    class ExtractDialog : public QDialog, public Ui::ExtractDialog
    {
        Q_OBJECT

    public:
        ExtractDialog(QWidget* parent, QString default_extract_dir);

    private slots:
        void slotChooseExtractDir();
    };

}
