
#pragma once

#include "ui_options.h"

namespace swganh {

    class OptionsDialog : public QDialog, public Ui::Options
    {
        Q_OBJECT

    public:
        OptionsDialog(QWidget* parent, QString project_directory);

        private slots:
        void slotChooseProjectDirectory();
        void slotDefaults();
    };

}
