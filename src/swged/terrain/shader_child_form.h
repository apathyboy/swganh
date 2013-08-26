
#pragma once

#include "swganh/terrain/terrain_types.h"
#include "ui_shader_child_form.h"

namespace swged {

	class ShaderChildForm : public QWidget, private Ui::ShaderChildForm
	{
		Q_OBJECT

	public:
		explicit ShaderChildForm(swganh::terrain::shader_family::shader_child* child, QWidget* parent = nullptr);
		~ShaderChildForm();

	private:
		swganh::terrain::shader_family::shader_child* child_;

	private slots:
		void weightChanged(const QString& value);
	};

}
