
#include "terrain_editor.h"
#include "project_manager.h"
#include "swganh/byte_buffer.h"
#include "swganh/tre/visitors/terrain/terrain_visitor.h"
#include "swganh/tre/iff/iff.h"
#include "swganh/tre/tre_archive.h"

namespace swganh {

	TerrainEditor::TerrainEditor(QString terrain_file, ProjectManager* project_manager, QWidget* parent)
		: QMainWindow(parent)
		, project_manager_(project_manager)
		, terrain_file_(terrain_file)
	{
		setupUi(this);

		auto archive = project_manager_->getArchive();
		terrain_visitor_ = std::make_shared<swganh::tre::TerrainVisitor>();

		swganh::tre::iff_file::loadIFF(archive->GetResource(terrain_file_.toStdString()), terrain_visitor_);

		setWindowTitle(windowTitle().append(" - ").append(terrain_file));
		layerTree->initialize(&terrain_visitor_->GetLayers());
	}

}
