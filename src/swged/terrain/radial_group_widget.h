
#pragma once

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

Q_DECLARE_METATYPE(swganh::terrain::radial_family*)
Q_DECLARE_METATYPE(swganh::terrain::radial_family::radial_child*)

namespace swged {

	class DDSPreview;
	
	class RadialGroupWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit RadialGroupWidget(QWidget* parent = nullptr);
		~RadialGroupWidget();

		void setRadialGroup(swganh::terrain::radial_group_t* radial_group);
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

		typedef GroupModel<swganh::terrain::radial_family, swganh::terrain::radial_family::radial_child> RadialGroupModel;
		std::unique_ptr<RadialGroupModel> model_;
		QPlainTextEdit* console_;

		swganh::tre::TreArchive* archive_;
	};

}
