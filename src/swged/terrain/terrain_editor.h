
#pragma once

#include <memory>

#include "ui_terrain_editor.h"

namespace swganh {
namespace terrain {
		struct procedural_terrain;
}}

namespace swganh {
namespace tre {
	class TreArchive;
}}


namespace swganh {
	class TerrainEditor : public QMainWindow, private Ui::TerrainEditor
	{
		Q_OBJECT

	public:
		TerrainEditor(std::unique_ptr<swganh::terrain::procedural_terrain> terrain, swganh::tre::TreArchive* archive, QWidget* parent = nullptr);
		~TerrainEditor();

	private slots:
		void onPropertiedItemSelected(QWidget* widget);

	private:
		std::unique_ptr<swganh::terrain::procedural_terrain> terrain_;
		swganh::tre::TreArchive* archive_;
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
