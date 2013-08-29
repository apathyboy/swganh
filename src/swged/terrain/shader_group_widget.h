
#pragma once

#include <memory>

#include <QImage>
#include <QString>
#include <QWidget>

#include "swganh/terrain/procedural_terrain.h"

#include "group_model.h"

namespace swganh {
	namespace tre {
		class TreArchive;
	}
}

class QToolBar;
class QTreeView;
class QPlainTextEdit;

Q_DECLARE_METATYPE(swganh::terrain::shader_family*)
Q_DECLARE_METATYPE(swganh::terrain::shader_family::shader_child*)

namespace swged {

	class DDSPreview;

	class ShaderGroupWidget : public QWidget
	{
		Q_OBJECT

	public:

		ShaderGroupWidget(QWidget* parent = nullptr);
		~ShaderGroupWidget();

		void setShaderGroup(swganh::terrain::shader_group_t* shader_group);
		void setConsole(QPlainTextEdit* console);
		void setArchive(swganh::tre::TreArchive* archive);

	signals:
		void propertiedItemSelected(QWidget*);

	private slots:
		void itemClicked(const QModelIndex&);

	private:

		QImage readDDSFile(const QString& filename);

		std::unique_ptr<QToolBar> toolbar_;
		std::unique_ptr<QTreeView> family_tree_;
		std::unique_ptr<DDSPreview> shader_preview_;
		//std::unique_ptr<ShaderGroupModel> model_;
		typedef GroupModel<swganh::terrain::shader_family, swganh::terrain::shader_family::shader_child> ShaderGroupModel;
		std::unique_ptr<ShaderGroupModel> model_;
		QPlainTextEdit* console_;

		swganh::tre::TreArchive* archive_;
	};

}
