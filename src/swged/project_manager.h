
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
	class TerrainEditor;

    class ProjectManager : public QObject
    {
        Q_OBJECT
    
    public:
        ProjectManager(MainWindow* parent, ProjectTree* tree_files);
        ~ProjectManager();

        QString getProjectDirectory() { return project_directory_; }

        bool openProject(QString project_directory);
        void closeProject();

		void openFile(QString project_file);

        tre::TreArchive* getArchive();

    signals:
        void closingProject();

    private:
        MainWindow* parent_;
        ProjectTree* tree_files_;
		std::unique_ptr<TerrainEditor> terrain_editor_;
        std::unique_ptr<tre::TreArchive> archive_;
        QString project_directory_;
    };

}
