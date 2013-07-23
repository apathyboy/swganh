
#include "project_manager.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <QMessageBox>
#include <QTreeWidget>

#include <boost/filesystem.hpp>

#include "swganh/tre/tre_archive.h"

#include "widgets/main_window.h"
#include "widgets/project_tree.h"
#include "terrain/terrain_editor.h"

namespace bf = boost::filesystem;

namespace swganh {

    ProjectManager::ProjectManager(MainWindow* parent, ProjectTree* tree_files)
        : parent_(parent)
        , tree_files_(tree_files)
    {
        tree_files_->setProjectManager(this);

        connect(this, SIGNAL(closingProject()), tree_files_, SLOT(clear()));
    }

    ProjectManager::~ProjectManager()
    {}

    bool ProjectManager::openProject(QString project_directory)
    {
        if (project_directory_.compare(project_directory) == 0) {
            return true;
        }

        if (bf::is_directory(project_directory.toStdString())) 
        {
            closeProject();

            project_directory_ = project_directory;
            archive_ = std::make_unique<tre::TreArchive>(project_directory.toStdString() + "/live.cfg");
            tree_files_->load(project_directory);

            return true;
        }

        return false;
    }

    void ProjectManager::closeProject()
    {
        project_directory_.clear();
        archive_.reset();

        emit closingProject();
    }


	void ProjectManager::openFile(QString project_file)
	{
		std::string extension = bf::extension(project_file.toStdString());
		if (extension.compare(".trn") == 0)
		{
			// open terrain editor
			terrain_editor_ = std::make_unique<TerrainEditor>(project_file, this);
			terrain_editor_->show();
		}
		else
		{
			// open as hex view
			QMessageBox msgBox;
			msgBox.setText("open as hex view.");
			msgBox.exec();
		}
	}

    tre::TreArchive* ProjectManager::getArchive()
    {
        return archive_.get();
    }

}
