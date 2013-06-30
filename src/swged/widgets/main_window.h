
#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QTreeWidget;
class QMenu;
class QMdiArea;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void about();

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void createDockWindows();

    QMdiArea* mdi_area_;
    QTreeWidget* resource_tree_;

    QMenu* file_menu_;
    QMenu* edit_menu_;
    QMenu* view_menu_;
    QMenu* help_menu_;
    QAction* about_action_;
    QAction* quit_action_;
};
