
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
	this->featherClampEdit->setText(QString::number(family_->feather_clamp));
	this->nameLabel->setText(QString::fromStdString(family_->family_name));

	for (const auto& child : family_->children)
	{
		this->childrenList->addItem(new QListWidgetItem(QString::fromStdString(child->name)));
	}

	connect(surfaceProperties, SIGNAL(textChanged(const QString&)), SLOT(surfacePropertiesChanged(const QString&)));
	connect(featherClampEdit, SIGNAL(textChanged(const QString&)), SLOT(featherClampChanged(const QString&)));
}

ShaderFamilyForm::~ShaderFamilyForm()
{}


void ShaderFamilyForm::surfacePropertiesChanged(const QString& properties_file)
{
	family_->surface_properties_file = properties_file.toStdString();
}

void ShaderFamilyForm::featherClampChanged(const QString& new_value)
{
	family_->feather_clamp = new_value.toFloat();
}
