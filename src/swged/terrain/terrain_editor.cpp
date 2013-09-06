
#include "terrain_editor.h"

#include <iostream>

#include <QDebug>
#include <QMessageBox>

#include "swganh/terrain/procedural_terrain.h"
#include "swganh/tre/tre_archive.h"

#include "layer_model.h"

using swganh::terrain::procedural_terrain;
using swganh::TerrainEditor;
using swganh::tre::TreArchive;


TerrainEditor::TerrainEditor(std::unique_ptr<procedural_terrain> terrain, TreArchive* archive, QWidget* parent)
	: QMainWindow(parent)
	, terrain_(std::move(terrain))
	, archive_(archive)
{
	setupUi(this);

    auto layer_model = new LayerModel(terrain_->layers, layerTree);
    layerTree->setModel(layer_model);

    //9tabShaderGroup->setConsole(consoleEdit);
	//9tabShaderGroup->setArchive(archive_);
	//9tabShaderGroup->setShaderGroup(&terrain_->shader_group);

	tabRadialGroup->setConsole(consoleEdit);
	tabRadialGroup->setArchive(archive_);
	tabRadialGroup->setRadialGroup(&terrain_->radial_group);

	connect(tabShaderGroup, SIGNAL(propertiedItemSelected(QWidget*)), this, SLOT(onPropertiedItemSelected(QWidget*)));

	//
	//	connect(layerTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
	//		this, SLOT(onLayerClicked(QTreeWidgetItem*, int)));
}

TerrainEditor::~TerrainEditor()
{}

void TerrainEditor::onPropertiedItemSelected(QWidget* widget)
{
	contextProperties->setWidget(widget);
}

//// old
//
//TerrainEditor::TerrainEditor(QString terrain_file, ProjectManager* project_manager, QWidget* parent)
//	: QMainWindow(parent)
//	, project_manager_(project_manager)
//	, terrain_file_(terrain_file)
//{
//	setupUi(this);
//
//	auto archive = project_manager_->getArchive();
//	terrain_visitor_ = std::make_shared<swganh::tre::TerrainVisitor>();
//
//	swganh::tre::iff_file::loadIFF(archive->GetResource(terrain_file_.toStdString()), terrain_visitor_);
//
//	setWindowTitle(windowTitle().append(" - ").append(terrain_file));
//	layerTree->initialize(&terrain_visitor_->GetLayers());
//
//	connect(layerTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
//		this, SLOT(onLayerClicked(QTreeWidgetItem*, int)));
//
//}
//
//
//void TerrainEditor::on_actionMap_Options_triggered()
//{
//	QDialog map_options;
//	Ui::MapOptions ui;
//	ui.setupUi(&map_options);
//
//	auto header = terrain_visitor_->GetHeader();
//	ui.mapWidth->setText(QString::number(header.map_width));
//	ui.chunkWidth->setText(QString::number(header.chunk_width));
//	ui.tilesPerChunk->setText(QString::number(header.tiles_per_chunk));
//	ui.useGlobalWaterHeightBox->setChecked((header.use_global_water_height != 0) ? true : false);
//	ui.globalWaterHeight->setText(QString::number(header.global_water_height));
//	ui.waterShader->setText(QString::fromStdString(header.water_shader_name));
//	ui.waterShaderSize->setText(QString::number(header.water_shader_size));
//
//	int hours = ((header.seconds_per_world_cycle / 60) / 60);
//	int mins = (((int)header.seconds_per_world_cycle / 60) % 60);
//	ui.cycleHours->setText(QString::number(hours));
//	ui.cycleMins->setText(QString::number(mins));
//
//	map_options.exec();
//}
//
//void TerrainEditor::on_actionFlora_Options_triggered()
//{
//	QDialog flora_options;
//	Ui::FloraOptions ui;
//	ui.setupUi(&flora_options);
//
//	auto header = terrain_visitor_->GetHeader();
//
//	ui.collidableMaxDistance->setText(QString::number(header.Collidable_MaxDist));
//	ui.collidableMinDistance->setText(QString::number(header.Collidable_MinDist));
//	ui.collidableTileSize->setText(QString::number(header.Collidable_TileSize));
//	ui.collidableTileBorder->setText(QString::number(header.Collidable_TileBorder));
//	ui.collidableSeed->setText(QString::number(header.Collidable_Seed));
//
//	ui.nonCollidableMaxDistance->setText(QString::number(header.NonCollidable_MaxDist));
//	ui.nonCollidableMinDistance->setText(QString::number(header.NonCollidable_MinDist));
//	ui.nonCollidableTileSize->setText(QString::number(header.NonCollidable_TileSize));
//	ui.nonCollidableTileBorder->setText(QString::number(header.NonCollidable_TileBorder));
//	ui.nonCollidableSeed->setText(QString::number(header.NonCollidable_Seed));
//
//	ui.nearMaxDistance->setText(QString::number(header.NearRadial_MaxDist));
//	ui.nearMinDistance->setText(QString::number(header.NearRadial_MinDist));
//	ui.nearTileSize->setText(QString::number(header.NearRadial_TileSize));
//	ui.nearTileBorder->setText(QString::number(header.NearRadial_TileBorder));
//	ui.nearSeed->setText(QString::number(header.NearRadial_Seed));
//
//	ui.farMaxDistance->setText(QString::number(header.FarRadial_MaxDist));
//	ui.farMinDistance->setText(QString::number(header.FarRadial_MinDist));
//	ui.farTileSize->setText(QString::number(header.FarRadial_TileSize));
//	ui.farTileBorder->setText(QString::number(header.FarRadial_TileBorder));
//	ui.farSeed->setText(QString::number(header.FarRadial_Seed));
//
//	flora_options.exec();
//}
//
//void TerrainEditor::onLayerClicked(QTreeWidgetItem* expanded_item, int column)
//{
//	auto layer = qvariant_cast<std::shared_ptr<Layer>>(expanded_item->data(0, Qt::UserRole));
//
//	switch (layer->GetType())
//	{
//	case LayerType::LAYER_TYPE_CONTAINER:
//		{
//			contextProperties->setWidget(new LayerPropertiesWidget(layer, this));
//		}
//		break;
//	case LayerType::LAYER_TYPE_BOUNDARY:
//	case LayerType::LAYER_TYPE_BOUNDARY_POLYGON:
//		{
//		}
//		break;
//	case LayerType::LAYER_TYPE_HEIGHT:
//		{
//		}
//		break;
//	case LayerType::LAYER_TYPE_FILTER:
//		{
//		}
//		break;
//	}
//}
