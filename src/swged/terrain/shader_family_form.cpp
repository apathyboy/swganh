
#include "shader_family_form.h"

#include "swganh/terrain/terrain_types.h"

using swganh::terrain::shader_family;
using swged::ShaderFamilyForm;

ShaderFamilyForm::ShaderFamilyForm(shader_family* family, QWidget* parent)
	: QWidget(parent)
	, family_(family)
{
	setupUi(this);

	this->surfaceProperties->setText(QString::fromStdString(family_->surface_properties_file));
	this->featherClamp->setValue(family_->feather_clamp);
	this->nameLabel->setText(QString::fromStdString(family_->family_name));

	auto rgbdata = QString::number(family_->r).append(", ")
		.append(QString::number(family_->g)).append(", ")
		.append(QString::number(family_->b));

	colorPreview->setStyleSheet(QString::fromStdString("background:rgb(").append(rgbdata).append(");"));

	for (const auto& child : family_->children)
	{
		this->childrenList->addItem(new QListWidgetItem(QString::fromStdString(child->name)));
	}

	connect(surfaceProperties, SIGNAL(textChanged(const QString&)), SLOT(surfacePropertiesChanged(const QString&)));
	connect(featherClamp, SIGNAL(valueChanged(int)), SLOT(featherClampChanged(int)));
}

ShaderFamilyForm::~ShaderFamilyForm()
{}


void ShaderFamilyForm::surfacePropertiesChanged(const QString& properties_file)
{
	family_->surface_properties_file = properties_file.toStdString();
}

void ShaderFamilyForm::featherClampChanged(int value)
{
	family_->feather_clamp = value;
}
