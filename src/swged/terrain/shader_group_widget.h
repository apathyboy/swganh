
#pragma once

#include <memory>

#include <QImage>
#include <QString>
#include <QWidget>

#include "swganh/terrain/procedural_terrain.h"

class QToolBar;
class QTreeView;
class QPlainTextEdit;

namespace swged {

	class ShaderGroupModel;
	class DDSPreview;

	class ShaderGroupWidget : public QWidget
	{
		Q_OBJECT

	public:

		ShaderGroupWidget(QWidget* parent = nullptr);
		~ShaderGroupWidget();

		void setShaderGroup(swganh::terrain::shader_group_t* shader_group);
		void setConsole(QPlainTextEdit* console);
	private slots:
		void itemClicked(const QModelIndex&);

	private:

		QImage readDDSFile(const QString& filename);

		std::unique_ptr<QToolBar> toolbar_;
		std::unique_ptr<QTreeView> family_tree_;
		std::unique_ptr<DDSPreview> shader_preview_;
		std::unique_ptr<ShaderGroupModel> model_;
		QPlainTextEdit* console_;
	};

}
