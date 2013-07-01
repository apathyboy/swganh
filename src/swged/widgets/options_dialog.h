
#pragma once

#include "ui_options.h"

class OptionsDialog : public QDialog, public Ui::Options
{
    Q_OBJECT

public:
    OptionsDialog(QWidget* parent, QString project_directory);

private slots:
    void slotChooseProjectDirectory();
    void slotDefaults();
};
