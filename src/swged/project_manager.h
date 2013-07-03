
#pragma once

#include <memory>
#include <QObject>

class QTreeWidgetItem;

namespace swganh {
namespace tre {
    class TreArchive;
}}  // namespace swganh::tre

namespace swganh {

    class MainWindow;
    class ProjectTree;

    class ProjectManager : public QObject
    {
        Q_OBJECT
    
    public:
        ProjectManager(MainWindow* parent, ProjectTree* tree_files);
        ~ProjectManager();

        void initialize(QString project_directory);

    private:
        void loadProjectTree(QString project_directory);

        MainWindow* parent_;
        ProjectTree* tree_files_;
        std::unique_ptr<tre::TreArchive> archive_;

    private slots:
        void slotProjectTreeCollapsed(QTreeWidgetItem* expanded_item);
        void slotProjectTreeExpanded(QTreeWidgetItem* expanded_item);
    };

}
