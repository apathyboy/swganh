
#pragma once

#include <set>
#include <string>
#include <QTreeWidget>

class QPoint;

namespace swganh {

    class ProjectManager;

    class ProjectTree : public QTreeWidget
    {
        Q_OBJECT

    public:
        ProjectTree(QWidget* parent = 0);

        void setProjectManager(ProjectManager* project_manager) { project_manager_ = project_manager; }

        void load(QString project_directory);

    public slots:
        void slotAddTreeItem(QString text, QString data, QTreeWidgetItem* parent = nullptr);

    signals:
        void newTreeItem(QString text, QString data, QTreeWidgetItem* parent = nullptr);

    private:
        ProjectManager* project_manager_;
		std::set<std::string> cache_;

        bool isFile(QTreeWidgetItem* item) const;
        bool isDir(QTreeWidgetItem* item) const;

    private slots:
        void slotItemExpanded(QTreeWidgetItem* expanded_item);
        void slotItemDoubleClicked(QTreeWidgetItem* expanded_item, int column);
        void slotExtractFile();
        void slotExtractDir();
        void slotContextMenuRequested(const QPoint& p);
    };

}
