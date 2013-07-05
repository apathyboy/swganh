
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

        void load(QString project_directory);

    private:
        ProjectManager* project_manager_;

    private slots:
        void slotItemCollapsed(QTreeWidgetItem* expanded_item);
        void slotItemExpanded(QTreeWidgetItem* expanded_item);
        void slotItemDoubleClicked(QTreeWidgetItem* expanded_item, int column);
    };

}
