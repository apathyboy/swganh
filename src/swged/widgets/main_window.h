
#pragma once

#include "ui_main_window.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);

    QString getProjectDirectory() { return project_directory_; }
    
protected:
    void closeEvent(QCloseEvent *event);

private:
	QString project_directory_;

    void setProjectDirectory(QString dir);

    void loadSettings();
    void saveSettings();

private slots:
	void slotOptions();
};
