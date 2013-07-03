
#pragma once

#include <QTreeWidget>

namespace swganh {

    class ProjectManager;

    class ProjectTree : public QTreeWidget
    {
        Q_OBJECT

    public:
        ProjectTree(QWidget* parent = 0);

        void setProjectManager(ProjectManager* project_manager) { project_manager_ = project_manager; }
        
    private:
        ProjectManager* project_manager_;
    };

}
