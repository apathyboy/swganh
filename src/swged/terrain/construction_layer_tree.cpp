
#include "construction_layer_tree.h"

#include "swganh/tre/visitors/terrain/terrain_visitor.h"

using swganh::tre::BoundaryLayer;
using swganh::tre::ContainerLayer;
using swganh::tre::FilterLayer;
using swganh::tre::HeightLayer;
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
		{
			auto container = static_cast<ContainerLayer*>(layer.get());
			item->setText(0, QString("Container"));
			for (const auto& child : container->children)
			{
				addLayer(child, item);
			}
			for (const auto& child : container->boundaries)
			{
				addLayer(child, item);
			}
			for (const auto& child : container->filters)
			{
				addLayer(child, item);
			}
			for (const auto& child : container->heights)
			{
				addLayer(child, item);
			}
		}
		break;
	case LayerType::LAYER_TYPE_BOUNDARY:
	case LayerType::LAYER_TYPE_BOUNDARY_POLYGON:
		{
			item->setText(0, QString("Boundary"));
		}
		break;
	case LayerType::LAYER_TYPE_HEIGHT:
		{
			item->setText(0, QString("Height"));
		}
		break;
	case LayerType::LAYER_TYPE_FILTER:
		{
			item->setText(0, QString("Filter"));
		}
		break;
	}
}
