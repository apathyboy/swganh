
#pragma once

#include "ui_main_window.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);

private:
	QString project_directory_;

    void setProjectDirectory(QString dir);

private slots:
	void slotOptions();
};
