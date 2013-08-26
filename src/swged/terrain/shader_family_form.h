
#pragma once

#include "ui_shader_family_form.h"

namespace swganh {
	namespace terrain {
		struct shader_family;
	}
}

namespace swged {

	class ShaderFamilyForm : public QWidget, private Ui::ShaderFamilyForm
	{
		Q_OBJECT

	public:
		explicit ShaderFamilyForm(swganh::terrain::shader_family* family, QWidget* parent = nullptr);
		~ShaderFamilyForm();

	private:
		swganh::terrain::shader_family* family_;

	private slots:
		void surfacePropertiesChanged(const QString& properties_file);
		void featherClampChanged(const QString& new_value);
	};

}
