
#pragma once

#include <QWidget>

#include "ui_form_layer_properties.h"
#include "swganh/tre/visitors/terrain/terrain_visitor.h"

namespace swganh {

	class LayerPropertiesWidget : public QWidget, Ui::LayerPropertiesForm
	{
		Q_OBJECT

	public:
		LayerPropertiesWidget(std::shared_ptr<tre::Layer> layer, QWidget* parent = nullptr);

		tre::ContainerLayer* getLayer() const { return layer_.get(); }

	private:
		std::shared_ptr<tre::ContainerLayer> layer_;
		std::shared_ptr<QWidget> widget_ui_;
	};

}