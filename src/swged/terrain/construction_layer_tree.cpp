
#include "construction_layer_tree.h"

#include "swganh/tre/visitors/terrain/terrain_visitor.h"

using swganh::tre::ContainerLayer;
using swganh::tre::Layer;
using swganh::tre::LayerType;
using swganh::ConstructionLayerTree;

ConstructionLayerTree::ConstructionLayerTree(QWidget* parent)
	: QTreeWidget(parent)
{}

void ConstructionLayerTree::initialize(std::vector<std::shared_ptr<ContainerLayer>>* layers)
{
	layers_ = layers;
	auto root = invisibleRootItem();

	for (const auto& layer : *layers_)
	{
		addLayer(layer, root);
	}
}

void ConstructionLayerTree::addLayer(std::shared_ptr<Layer> layer, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(parent);
	item->setData(0, Qt::UserRole, QVariant::fromValue(layer));

	item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
	if (layer->enabled)
	{
		item->setCheckState(0, Qt::Checked);
	}
	else
	{
		item->setCheckState(0, Qt::Unchecked);
	}

	switch (layer->GetType())
	{
	case LayerType::LAYER_TYPE_CONTAINER:
		//auto container = std::static_pointer_cast<ContainerLayer>(layer);
		item->setText(0, QString("Container"));
		break;
	case LayerType::LAYER_TYPE_BOUNDARY:
		item->setText(0, QString("Boundary"));
		break;
	case LayerType::LAYER_TYPE_BOUNDARY_POLYGON:
		item->setText(0, QString("Boundary Polygon"));
		break;
	case LayerType::LAYER_TYPE_HEIGHT:
		item->setText(0, QString("Height"));
		break;
	case LayerType::LAYER_TYPE_FILTER:
		item->setText(0, QString("Filter"));
		break;
	}
}
