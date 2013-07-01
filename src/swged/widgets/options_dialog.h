
#pragma once

#include "ui_options.h"

class OptionsDialog : public QDialog, public Ui::Options
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget* parent);

private slots:
    void slotChooseProjectDirectory();
};
