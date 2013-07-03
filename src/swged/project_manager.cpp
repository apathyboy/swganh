
#include "project_manager.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <QMessageBox>
#include <QTreeWidget>

#include "swganh/tre/tre_archive.h"

#include "widgets/main_window.h"
#include "widgets/project_tree.h"

namespace swganh {

    ProjectManager::ProjectManager(MainWindow* parent, ProjectTree* tree_files)
        : parent_(parent)
        , tree_files_(tree_files)
    {
        tree_files_->setProjectManager(this);
    }

    ProjectManager::~ProjectManager()
    {}

    void ProjectManager::initialize(QString project_directory)
    {
        archive_ = std::make_unique<tre::TreArchive>(project_directory.toStdString() + "/live.cfg");

        tree_files_->load(project_directory);
    }

    tre::TreArchive* ProjectManager::getArchive()
    {
        return archive_.get();
    }

}
