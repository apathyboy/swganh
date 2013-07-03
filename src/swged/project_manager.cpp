
#include "project_manager.h"

#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

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
        tree_files_->clear();
        archive_ = std::make_unique<tre::TreArchive>(project_directory.toStdString() + "/live.cfg");

        loadProjectTree(project_directory);
    }

    void ProjectManager::loadProjectTree(QString project_directory)
    {
        QTreeWidgetItem* project_root = new QTreeWidgetItem;

        QString projectName = project_directory.section("/", -1, -1);
        project_root->setText(0, projectName);
        project_root->setData(0, Qt::UserRole, project_directory);
        
        std::map<std::string, QTreeWidgetItem*> item_cache;

        archive_->VisitAvailableResources([project_root, &item_cache] (std::string resource_name) {
            std::vector<std::string> path_data;
            std::string current_depth;
            boost::split(path_data, resource_name, boost::is_any_of("/"));
            
            QTreeWidgetItem* parent = nullptr;
            
            auto path_depth = path_data.size();
            for (uint32_t i = 0; i < path_depth; ++i)
            {
                const auto& current_item = path_data[i];
                current_depth += "/" + current_item;
            
                if (i == 0)
                {
                    parent = project_root;
                }
            
                auto find_iter = item_cache.find(current_depth);
                if (find_iter != item_cache.end())
                {
                    parent = find_iter->second;
                }
                else
                {
                    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
            
                    item->setText(0, QString().fromStdString(current_item));
                    item->setData(0, Qt::UserRole, QString().fromStdString(resource_name));
            
                    parent = item;
                    item_cache.insert(std::make_pair(current_depth, item));
                }
            }
        });

        tree_files_->addTopLevelItem(project_root);
        tree_files_->expandItem(project_root);
    }

}
