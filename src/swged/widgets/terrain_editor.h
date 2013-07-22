
#pragma once

#include "ui_terrain_editor.h"

namespace swganh {

	class ProjectManager;

	class TerrainEditor : public QMainWindow, private Ui::TerrainEditor
	{
		Q_OBJECT

	public:
		TerrainEditor(QWidget* parent = nullptr);
	};

}
