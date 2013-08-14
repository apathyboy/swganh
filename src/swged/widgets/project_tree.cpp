
#include "project_tree.h"

#include <fstream>
#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QtConcurrent>
#include <QProgressDialog>

#include "swganh/tre/tre_archive.h"

#include "widgets/extract_dialog.h"
#include "project_manager.h"

namespace bfs = boost::filesystem;

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
        setUpdatesEnabled(false);

        project_manager_->getArchive()->VisitAvailableResources(
            [this](std::string resource_name)
        {
            std::vector<std::string> path_data;
            boost::split(path_data, resource_name, boost::is_any_of("/"));

            if (cache_.find(path_data[0]) == cache_.end())
            {
				slotAddTreeItem(QString().fromStdString(path_data[0]),
					QString().fromStdString(path_data[0]));

                cache_.insert(path_data[0]);
            }
        });
        setUpdatesEnabled(true);
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
		setUpdatesEnabled(false);
        auto item_data = expanded_item->data(0, Qt::UserRole).toString().toStdString();
		auto item_text = expanded_item->text(0).toStdString();

        project_manager_->getArchive()->VisitAvailableResources(
			[this, &item_data, &item_text, expanded_item] (std::string resource_name)
        {
            if (resource_name.compare(0, item_data.size(), item_data) == 0)
            {
                auto substr = resource_name.substr(item_data.size() + 1);
                std::vector<std::string> path_data;
                boost::split(path_data, substr, boost::is_any_of("/"));

				auto new_item_text = path_data[0];
				auto new_item_data = item_data + "/" + path_data[0];

				if (cache_.find(new_item_data) == cache_.end())
                {
					slotAddTreeItem(QString().fromStdString(new_item_text),
						QString().fromStdString(new_item_data), expanded_item);

					cache_.insert(new_item_data);
                }
            }
		});
		setUpdatesEnabled(true);
    }

    void ProjectTree::slotItemDoubleClicked(QTreeWidgetItem* expanded_item, int column)
    {
        auto text = expanded_item->text(0);
        if (text.toStdString().find_first_of(".") != std::string::npos)
        {
			project_manager_->openFile(expanded_item->data(0, Qt::UserRole).toString());
        }
    }

    void ProjectTree::slotExtractFile()
    {
        auto current_item = currentItem();
        auto project_dir = project_manager_->getProjectDirectory();

        ExtractDialog* extract = new ExtractDialog(this, project_manager_->getProjectDirectory());

        if (extract->exec() == QDialog::Accepted)
        {
            auto resource_name = current_item->data(0, Qt::UserRole).toString();
            auto resource = project_manager_->getArchive()->GetResource(resource_name.toStdString());
            auto target_filename = project_dir.append("/").append(resource_name);

            // Create a progress dialog.
            QProgressDialog dialog;
            dialog.setLabelText(QString("Extracting %1").arg(resource_name));

            // Create a QFutureWatcher and connect signals and slots.
            QFutureWatcher<void> futureWatcher;
            QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
            QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
            QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
            QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));


            futureWatcher.setFuture(QtConcurrent::run([target_filename, resource] ()
            {
                auto path = bfs::path(target_filename.toStdString());

                if (bfs::is_regular_file(path))
                {
                    bfs::remove(path);
                }

                if (!bfs::is_directory(path.parent_path()))
                {
                    bfs::create_directories(path.parent_path());
                }

                std::ofstream ofs(path.string(), std::ios::out | std::ios::binary);

                if (resource.size() > 0)
                {
                    ofs.write(reinterpret_cast<const char*>(resource.data()), resource.size());
                }

                ofs.close();
            }));

            dialog.exec();
            futureWatcher.waitForFinished();
        }
    }

    void ProjectTree::slotExtractDir()
    {
        auto current_item = currentItem();
        auto project_dir = project_manager_->getProjectDirectory();

        ExtractDialog* extract = new ExtractDialog(this, project_manager_->getProjectDirectory());

        if (extract->exec() == QDialog::Accepted)
        {
            auto resource_name = current_item->data(0, Qt::UserRole).toString();

            // Create a progress dialog.
            QProgressDialog dialog;
            dialog.setLabelText(QString("Extracting %1").arg(resource_name));

            // Create a QFutureWatcher and connect signals and slots.
            QFutureWatcher<void> futureWatcher;
            QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
            QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
            QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
            QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

            QVector<QString> files;

            project_manager_->getArchive()->VisitAvailableResources(
                [&files, &resource_name](std::string stored_name)
            {
                if (stored_name.compare(0, resource_name.length(), resource_name.toStdString()) == 0)
                {
                    files.push_back(QString().fromStdString(stored_name));
                }
            });

            futureWatcher.setFuture(QtConcurrent::map(files, [this, project_dir](QString resource_name)
            {
                auto target_filename = project_dir;
                target_filename.append("/").append(resource_name);
                auto path = bfs::path(target_filename.toStdString());
                auto resource = project_manager_->getArchive()->GetResource(resource_name.toStdString());

                if (bfs::is_regular_file(path))
                {
                    bfs::remove(path);
                }

                if (!bfs::is_directory(path.parent_path()))
                {
                    bfs::create_directories(path.parent_path());
                }

                std::ofstream ofs(path.string(), std::ios::out | std::ios::binary);

                if (resource.size() > 0)
                {
                    ofs.write(reinterpret_cast<const char*>(resource.data()), resource.size());
                }

                ofs.close();
            }));

            dialog.exec();
            futureWatcher.waitForFinished();
        }
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
