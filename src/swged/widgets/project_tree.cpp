
#include "project_tree.h"

#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>

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

        setContextMenuPolicy(Qt::CustomContextMenu);

        connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotContextMenuRequested(const QPoint&)));


        connect(this, &ProjectTree::newTreeItem, this, &ProjectTree::slotAddTreeItem);
    }

    void ProjectTree::load(QString project_directory)
    {
        setRootIsDecorated(true);

        std::set<std::string> cache;

        project_manager_->getArchive()->VisitAvailableResources(
            [this, &cache](std::string resource_name)
        {
            std::vector<std::string> path_data;
            boost::split(path_data, resource_name, boost::is_any_of("/"));

            if (cache.find(path_data[0]) == cache.end())
            {
                emit newTreeItem(QString().fromStdString(path_data[0]),
                    QString().fromStdString(path_data[0]));

                cache.insert(path_data[0]);
            }
        });
    }

    void ProjectTree::slotAddTreeItem(QString text, QString data, QTreeWidgetItem* parent)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(parent);

        item->setText(0, text);
        item->setData(0, Qt::UserRole, data);

        if (!text.contains("."))
        {
            item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        }

        if (!parent)
        {
            addTopLevelItem(item);
        }
    }

    bool ProjectTree::isFile(QTreeWidgetItem* item) const
    {
        return item->text(0).contains(".");
    }

    bool ProjectTree::isDir(QTreeWidgetItem* item) const
    {
        return !isFile(item);
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
            [this, &item_data, expanded_item, &cache] (std::string resource_name) 
        {
            if (resource_name.compare(0, item_data.size(), item_data) == 0)
            {
                auto substr = resource_name.substr(item_data.size() + 1);
                std::vector<std::string> path_data;
                boost::split(path_data, substr, boost::is_any_of("/"));

                if (cache.find(path_data[0]) == cache.end())
                {
                    emit newTreeItem(QString().fromStdString(path_data[0]),
                        QString().fromStdString(item_data + "/" + path_data[0]), expanded_item);

                    cache.insert(path_data[0]);
                }
            }
        });
    }

    void ProjectTree::slotItemDoubleClicked(QTreeWidgetItem* expanded_item, int column)
    {
        auto text = expanded_item->text(0);
        if (text.toStdString().find_first_of(".") != std::string::npos)
        {
            QMessageBox msgBox;
            msgBox.setText("Open with default editor.");
            msgBox.exec();
        }
    }

    void ProjectTree::slotExtractFile()
    {
        QMessageBox msgBox;
        msgBox.setText("Extract file.");
        msgBox.exec();
    }

    void ProjectTree::slotExtractDir()
    {
        QMessageBox msgBox;
        msgBox.setText("Extract directory.");
        msgBox.exec();
    }

    void ProjectTree::slotContextMenuRequested(const QPoint&)
    {
        auto current_item = currentItem();

        QMenu* menu = new QMenu(this);

        if (isFile(current_item))
        {
            connect(menu->addAction(tr("Extract file...")), SIGNAL(triggered()), this, SLOT(slotExtractFile()));
        }
        else
        {
            connect(menu->addAction(tr("Extract directory...")), SIGNAL(triggered()), this, SLOT(slotExtractDir()));
        }

        menu->exec(QCursor::pos());
    }

}
