
#pragma once

#include <QObject>

namespace swganh {

    class MainWindow;
    class ProjectTree;

    class ProjectManager : public QObject
    {
        Q_OBJECT
    
    public:
        ProjectManager(MainWindow* parent, ProjectTree* tree_files);
        ~ProjectManager();

    private:
        MainWindow* parent_;
        ProjectTree* tree_files_;
    };

}
