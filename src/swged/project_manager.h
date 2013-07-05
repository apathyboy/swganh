
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

        QString getProjectDirectory() { return project_directory_; }

        void openProject(QString project_directory);
        void closeProject();

        tre::TreArchive* getArchive();

    private:
        MainWindow* parent_;
        ProjectTree* tree_files_;
        std::unique_ptr<tre::TreArchive> archive_;
        QString project_directory_;
    };

}
