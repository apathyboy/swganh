
#pragma once

#include <memory>

#include "ui_terrain_editor.h"

namespace swganh {

	namespace tre {
		class TerrainVisitor;
	}

	class ProjectManager;

	class TerrainEditor : public QMainWindow, private Ui::TerrainEditor
	{
		Q_OBJECT

	public:
		TerrainEditor(QString terrain_file, ProjectManager* project_manager, QWidget* parent = nullptr);


	private slots:
		void on_actionMap_Options_triggered();
		void on_actionFlora_Options_triggered();

	private:
		ProjectManager* project_manager_;
		std::shared_ptr<swganh::tre::TerrainVisitor> terrain_visitor_;
		QString terrain_file_;
	};

}
