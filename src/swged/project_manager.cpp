
#include "project_manager.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeView>

#include <boost/filesystem.hpp>

#include "swganh/utilities.h"
#include "swganh/tre/tre_archive.h"
#include "swganh/tre/iff.h"
#include "swganh/tre/terrain/procedural_terrain.h"

#include "widgets/main_window.h"
#include "widgets/project_tree.h"
#include "widgets/iff_tree_model.h"
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

            archive_ = swganh::make_unique<tre::TreArchive>(project_directory.toStdString() + "/live.cfg");

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
        if (!isDocumentOpen(project_file))
		{
            auto resource = archive_->GetResource(project_file.toStdString());

            if (extension.compare(".trn") == 0)
            {
                terrain_editor_ = swganh::make_unique<TerrainEditor>(
                    swganh::tre::read_procedural_terrain(resource));
                terrain_editor_->show();
            }
            else
            {
                auto tab_doc = new QTreeView(parent_->documentsTabWidget);
                tab_doc->setModel(new IffTreeModel(swganh::tre::parse_iff(resource)));

                parent_->documentsTabWidget->addTab(tab_doc, project_file);
                parent_->documentsTabWidget->setCurrentWidget(tab_doc);
                tab_doc->show();
            }
		}
        else
        {
            parent_->documentsTabWidget->setCurrentIndex(documentIndex(project_file));
        }
	}

    tre::TreArchive* ProjectManager::getArchive()
    {
        return archive_.get();
    }

    bool ProjectManager::isDocumentOpen(const QString& document) const
    {
        return documentIndex(document) >= 0;
    }

    int ProjectManager::documentIndex(const QString& document) const
    {
        auto count = parent_->documentsTabWidget->count();

        for (int i = 0; i < count; ++i)
        {
            if (parent_->documentsTabWidget->tabText(i).compare(document) == 0)
            {
                return i;
            }
        }

        return -1;
    }
}
