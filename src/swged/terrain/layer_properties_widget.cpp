
#include "layer_properties_widget.h"
#include "ui_form_layer_properties.h"

using swganh::LayerPropertiesWidget;
using swganh::tre::Layer;
using swganh::tre::ContainerLayer;
using swganh::tre::LayerType;

LayerPropertiesWidget::LayerPropertiesWidget(std::shared_ptr<Layer> layer, QWidget* parent)
	: QWidget(parent)
	, layer_(std::static_pointer_cast<ContainerLayer>(layer))
{
	setupUi(this);

	nameViewLabel->setText(QString::fromStdString(layer_->name));
	invertBoundariesCheckBox->setChecked(layer_->invert_boundaries);
	invertFiltersCheckBox->setChecked(layer_->invert_filters);
	notesEdit->setText(QString::fromStdString(layer_->notes));
}
