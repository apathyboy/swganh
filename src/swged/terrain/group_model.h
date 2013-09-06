
#pragma once

#include <QAbstractItemModel>

#include "swganh/terrain/procedural_terrain.h"

namespace swged {

	template<typename FamilyT, typename ChildT>
	class GroupModel : public QAbstractItemModel
	{
	public:
		explicit GroupModel(swganh::terrain::terrain_group<FamilyT>* group, QObject* parent = nullptr)
			: QAbstractItemModel(parent)
			, group_(group)
		{}
		
		~GroupModel()
		{}

		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override
		{
			if (parent.isValid())
			{
				auto family = typeFromIndex<FamilyT>(parent);
				return createIndex(row, column, family->children[row].get());
			}

			return createIndex(row, column, group_->get_families()[row].get());
		}

		QModelIndex parent(const QModelIndex& index) const override
		{
			// Families don't have parents, nor do invalid indexes.
			if (!index.isValid() || isFamily(index))
			{
				return QModelIndex();
			}

			auto child = typeFromIndex<ChildT>(index);
			int row = findFamilyOffset(child->parent);

			return createIndex(row, index.column(), child->parent);
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const override
		{
			if (parent.isValid())
			{
				if (isFamily(parent))
				{
					auto family = typeFromIndex<FamilyT>(parent);
					return family->children.size();
				}
				else
				{
					return 0;
				}
			}

			return group_->get_families().size();
		}

		int columnCount(const QModelIndex& parent = QModelIndex()) const override
		{
			return 1;
		}

		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
		{
			if (index.column() != 0)
			{
				return QVariant();
			}

			if (isFamily(index))
			{
				return getFamilyData(index, role);
			}

			return getFamilyChildData(index, role);
		}

		Qt::ItemFlags flags(const QModelIndex& index) const override
		{
			Qt::ItemFlags flags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
			return flags;
		}

	private:

		QVariant getFamilyData(const QModelIndex& index, int role) const
		{
			if (!index.isValid())
			{
				return QVariant();
			}

			auto family = typeFromIndex<FamilyT>(index);

			if (role == Qt::DisplayRole)
			{
				return QString::fromStdString(family->family_name);
			}
			else if (role == Qt::UserRole)
			{
				return QVariant::fromValue(family);
			}

			return QVariant();
		}

		QVariant getFamilyChildData(const QModelIndex& index, int role) const
		{
			if (!index.isValid())
			{
				return QVariant();
			}

			auto child = typeFromIndex<ChildT>(index);

			if (role == Qt::DisplayRole)
			{
				return QString::fromStdString(child->name);
			}
			else if (role == Qt::UserRole)
			{
				return QVariant::fromValue(child);
			}
			
			return QVariant();
		}

		int findFamilyOffset(FamilyT* family) const
		{
			const auto& families = group_->get_families();

			auto find_iter = std::find_if(families.begin(), families.end(),
				[family](const std::unique_ptr<FamilyT>& item)
			{
				return item.get() == family;
			});

			return find_iter - families.begin();
		}

		template<typename T>
		T* typeFromIndex(const QModelIndex& index) const
		{
			if (index.isValid())
			{
				return static_cast<T*>(index.internalPointer());
			}

			return nullptr;
		}

		bool isFamily(const QModelIndex& index) const
		{
			if (!index.isValid())
			{
				return false;
			}

			auto internal_ptr = index.internalPointer();

			for (const auto& family : group_->get_families())
			{
				if (family.get() == internal_ptr) return true;
			}

			return false;
		}

		bool isFamilyChild(const QModelIndex& index) const
		{
			if (!index.isValid())
			{
				return false;
			}

			return !isFamily(index);
		}

		swganh::terrain::terrain_group<FamilyT>* group_;
	};

}
