
#pragma once

#include <vector>

#include <QAbstractItemModel>

#include "swganh/terrain/procedural_terrain.h"

Q_DECLARE_METATYPE(swganh::terrain::shader_family*)
Q_DECLARE_METATYPE(swganh::terrain::shader_family::shader_child*)

namespace swged {

	class ShaderGroupModel : public QAbstractItemModel
	{
	public:
		explicit ShaderGroupModel(swganh::terrain::shader_group_t* shader_group, QObject* parent = nullptr);
		~ShaderGroupModel();

		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
		QModelIndex parent(const QModelIndex& index) const override;
		int rowCount(const QModelIndex& parent = QModelIndex()) const override;

		// layer name | layer type | layer sub type
		int columnCount(const QModelIndex& parent = QModelIndex()) const override;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

		Qt::ItemFlags flags(const QModelIndex& index) const override;

	private:
		void buildLookupTable();

		int getIndexType(const QModelIndex& index) const;
		quint32 getIndex(void* ptr) const;

		swganh::terrain::shader_family* familyFromIndex(const QModelIndex& index) const;
		swganh::terrain::shader_family::shader_child* childFromIndex(const QModelIndex& index) const;
		
		swganh::terrain::shader_group_t* shader_group_;
		std::vector<std::pair<uint32_t, void*>> lookup_table_;
	};

}
