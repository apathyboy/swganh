
#pragma once

#include <memory>

#include <QWidget>

#include "swganh/terrain/procedural_terrain.h"

class QToolBar;
class QTreeView;
class QGraphicsView;

namespace swged {

	class ShaderGroupModel;

	class ShaderGroupWidget : public QWidget
	{
		Q_OBJECT

	public:
		
		ShaderGroupWidget(QWidget* parent = nullptr);
		~ShaderGroupWidget();

		void setShaderGroup(swganh::terrain::shader_group_t* shader_group);

	private:

		std::unique_ptr<QToolBar> toolbar_;
		std::unique_ptr<QTreeView> family_tree_;
		std::unique_ptr<QGraphicsView> shader_preview_;
		std::unique_ptr<ShaderGroupModel> model_;
	};

}