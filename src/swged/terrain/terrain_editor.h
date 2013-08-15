
#pragma once

#include <memory>

#include "ui_terrain_editor.h"

namespace swganh {

	namespace tre {
		struct procedural_terrain;
	}


	class TerrainEditor : public QMainWindow, private Ui::TerrainEditor
	{
		Q_OBJECT

	public:
		explicit TerrainEditor(std::unique_ptr<swganh::tre::procedural_terrain> terrain, QWidget* parent = nullptr);
		~TerrainEditor();

	private:
		std::unique_ptr<swganh::tre::procedural_terrain> terrain_;
	};


	/// old



//	class ProjectManager;
//
//	class TerrainEditor : public QMainWindow, private Ui::TerrainEditor
//	{
//		Q_OBJECT
//
//	public:
//		TerrainEditor(QString terrain_file, ProjectManager* project_manager, QWidget* parent = nullptr);
//
//
//	private slots:
//		void on_actionMap_Options_triggered();
//		void on_actionFlora_Options_triggered();
//		void onLayerClicked(QTreeWidgetItem* expanded_item, int column);
//
//	private:
//		ProjectManager* project_manager_;
//		std::shared_ptr<swganh::tre::TerrainVisitor> terrain_visitor_;
//		QString terrain_file_;
//	};
}
