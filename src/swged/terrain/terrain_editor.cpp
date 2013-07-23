
#include "terrain_editor.h"
#include "project_manager.h"
#include "ui_map_options.h"
#include "swganh/byte_buffer.h"
#include "swganh/tre/visitors/terrain/terrain_visitor.h"
#include "swganh/tre/iff/iff.h"
#include "swganh/tre/tre_archive.h"

using swganh::TerrainEditor;
using swganh::ProjectManager;

TerrainEditor::TerrainEditor(QString terrain_file, ProjectManager* project_manager, QWidget* parent)
	: QMainWindow(parent)
	, project_manager_(project_manager)
	, terrain_file_(terrain_file)
{
	setupUi(this);

	auto archive = project_manager_->getArchive();
	terrain_visitor_ = std::make_shared<swganh::tre::TerrainVisitor>();

	swganh::tre::iff_file::loadIFF(archive->GetResource(terrain_file_.toStdString()), terrain_visitor_);

	setWindowTitle(windowTitle().append(" - ").append(terrain_file));
	layerTree->initialize(&terrain_visitor_->GetLayers());
}


void TerrainEditor::on_actionMap_Options_triggered()
{
	QDialog map_options;
	Ui::MapOptions ui;
	ui.setupUi(&map_options);

	auto header = terrain_visitor_->GetHeader();
	ui.mapWidth->setText(QString::number(header.map_width));
	ui.chunkWidth->setText(QString::number(header.chunk_width));
	ui.tilesPerChunk->setText(QString::number(header.tiles_per_chunk));
	ui.useGlobalWaterHeightBox->setChecked((header.use_global_water_height != 0) ? true : false);
	ui.globalWaterHeight->setText(QString::number(header.global_water_height));
	ui.waterShader->setText(QString::fromStdString(header.water_shader_name));
	ui.waterShaderSize->setText(QString::number(header.water_shader_size));

	int hours = ((header.seconds_per_world_cycle / 60) / 60);
	int mins = (((int)header.seconds_per_world_cycle / 60) % 60);
	ui.cycleHours->setText(QString::number(hours));
	ui.cycleMins->setText(QString::number(mins));

	map_options.exec();
}