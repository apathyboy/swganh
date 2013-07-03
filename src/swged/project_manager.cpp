
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

        connect(tree_files_, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(slotProjectTreeCollapsed(QTreeWidgetItem*)));
        connect(tree_files_, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotProjectTreeExpanded(QTreeWidgetItem*)));
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
        tree_files_->setRootIsDecorated(true);

        QTreeWidgetItem* project_root = new QTreeWidgetItem;
        project_root->setText(0, "Game Data");
        project_root->setData(0, Qt::UserRole, project_directory);
        
        std::set<std::string> cache;

        archive_->VisitAvailableResources([this, project_root, &cache](std::string resource_name) {
            std::vector<std::string> path_data;
            boost::split(path_data, resource_name, boost::is_any_of("/"));

            if (cache.find(path_data[0]) == cache.end())
            {
                QTreeWidgetItem* item = new QTreeWidgetItem();

                item->setText(0, QString().fromStdString(path_data[0]));
                item->setData(0, Qt::UserRole, QString().fromStdString(path_data[0]));
                item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

                tree_files_->addTopLevelItem(item);

                cache.insert(path_data[0]);
            }
        });
    }

    void ProjectManager::slotProjectTreeCollapsed(QTreeWidgetItem* expanded_item)
    {
        for (int i = expanded_item->childCount() - 1; i >= 0; --i)
        {
            expanded_item->removeChild(expanded_item->child(i));
        }
    }

    void ProjectManager::slotProjectTreeExpanded(QTreeWidgetItem* expanded_item)
    {
        auto item_data = expanded_item->data(0, Qt::UserRole).toString().toStdString();

        std::set<std::string> cache;

        archive_->VisitAvailableResources([&item_data, expanded_item, &cache](std::string resource_name) {
            if (resource_name.compare(0, item_data.size(), item_data) == 0)
            {
                auto substr = resource_name.substr(item_data.size() + 1);
                std::vector<std::string> path_data;
                boost::split(path_data, substr, boost::is_any_of("/"));


                if (cache.find(path_data[0]) == cache.end())
                {
                    QTreeWidgetItem* item = new QTreeWidgetItem(expanded_item);

                    item->setText(0, QString().fromStdString(path_data[0]));
                    item->setData(0, Qt::UserRole, QString().fromStdString(item_data + "/" + path_data[0]));
                    
                    if (path_data[0].find_first_of(".") == std::string::npos)
                    {
                        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
                    }

                    cache.insert(path_data[0]);
                }
            }
        });
    }
}
