
#include "project_tree.h"

#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <QMessageBox>

#include "swganh/tre/tre_archive.h"

#include "project_manager.h"

namespace swganh {

    ProjectTree::ProjectTree(QWidget* parent)
        : QTreeWidget(parent)
    {
        connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(slotItemCollapsed(QTreeWidgetItem*)));
        connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));

        // launch editor with src file loaded
        connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*, int)));
    }

    void ProjectTree::load(QString project_directory)
    {
        setRootIsDecorated(true);

        QTreeWidgetItem* project_root = new QTreeWidgetItem;
        project_root->setText(0, "Game Data");
        project_root->setData(0, Qt::UserRole, project_directory);

        std::set<std::string> cache;

        project_manager_->getArchive()->VisitAvailableResources(
            [this, project_root, &cache](std::string resource_name)
        {
            std::vector<std::string> path_data;
            boost::split(path_data, resource_name, boost::is_any_of("/"));

            if (cache.find(path_data[0]) == cache.end())
            {
                QTreeWidgetItem* item = new QTreeWidgetItem();

                item->setText(0, QString().fromStdString(path_data[0]));
                item->setData(0, Qt::UserRole, QString().fromStdString(path_data[0]));
                item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

                addTopLevelItem(item);

                cache.insert(path_data[0]);
            }
        });
    }

    void ProjectTree::slotItemCollapsed(QTreeWidgetItem* expanded_item)
    {
        for (int i = expanded_item->childCount() - 1; i >= 0; --i)
        {
            expanded_item->removeChild(expanded_item->child(i));
        }
    }

    void ProjectTree::slotItemExpanded(QTreeWidgetItem* expanded_item)
    {
        auto item_data = expanded_item->data(0, Qt::UserRole).toString().toStdString();

        std::set<std::string> cache;

        project_manager_->getArchive()->VisitAvailableResources(
            [&item_data, expanded_item, &cache] (std::string resource_name) 
        {
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

    void ProjectTree::slotItemDoubleClicked(QTreeWidgetItem* expanded_item, int column)
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.exec();
    }
}
