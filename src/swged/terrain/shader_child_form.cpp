
#include "shader_child_form.h"

using swganh::terrain::shader_family;
using swged::ShaderChildForm;

ShaderChildForm::ShaderChildForm(swganh::terrain::shader_family::shader_child* child, QWidget* parent)
	: QWidget(parent)
	, child_(child)
{
	setupUi(this);

	familyNameEdit->setText(QString::fromStdString(child_->parent->family_name));
	familyNameEdit->setDisabled(true);

	childNameLabel->setText(QString::fromStdString(child_->name));
	weightEdit->setText(QString::number(child_->weight));
}

ShaderChildForm::~ShaderChildForm()
{}

void ShaderChildForm::weightChanged(const QString& value)
{
	child_->weight = value.toFloat();
}
