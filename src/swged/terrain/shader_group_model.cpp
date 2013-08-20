
#include "shader_group_model.h"

using swged::ShaderGroupModel;
using swganh::terrain::shader_group_t;
using swganh::terrain::shader_family;

ShaderGroupModel::ShaderGroupModel(shader_group_t* shader_group, QObject* parent)
	: QAbstractItemModel(parent)
	, shader_group_(shader_group)
{
	buildLookupTable();
}

ShaderGroupModel::~ShaderGroupModel()
{}

QModelIndex ShaderGroupModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		auto family = familyFromIndex(parent);
		return createIndex(row, column, getIndex(family->children[row].get()));
	}

	return createIndex(row, column, getIndex(shader_group_->get_families()[row].get()));
}

QModelIndex ShaderGroupModel::parent(const QModelIndex& index) const
{
	if (!index.isValid() || getIndexType(index) == 0)
	{
		return QModelIndex();
	}

	auto child = childFromIndex(index);

	const auto& families = shader_group_->get_families();

	auto find_iter = std::find_if(std::begin(families), std::end(families), 
		[child] (const std::unique_ptr<shader_family>& family)
	{
		return family.get() == child->parent;
	});

	int row = find_iter - std::begin(families);

	return createIndex(row, index.column(), getIndex(child->parent));
}

int ShaderGroupModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		if (getIndexType(parent) == 0)
		{
			auto family = familyFromIndex(parent);
			return family->children.size();
		}
		else
		{
			return 0;
		}
	}

	return shader_group_->get_families().size();
}

int ShaderGroupModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant ShaderGroupModel::data(const QModelIndex& index, int role) const
{
	if (role != Qt::DisplayRole || index.column() != 0)
	{
		return QVariant();
	}

	if (getIndexType(index) == 0)
	{
		auto family = familyFromIndex(index);
		return QString::fromStdString(family->family_name);
	}
	else
	{
		auto child = childFromIndex(index);
		return QString::fromStdString(child->name);
	}

	return QVariant();
}

Qt::ItemFlags ShaderGroupModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
	return flags;
}

void ShaderGroupModel::buildLookupTable()
{
	const auto& families = shader_group_->get_families();

	for (const auto& family : families)
	{
		lookup_table_.push_back(std::make_pair(0, family.get()));

		for (const auto& child : family->children)
		{
			lookup_table_.push_back(std::make_pair(1, child.get()));
		}
	}
}

int ShaderGroupModel::getIndexType(const QModelIndex& index) const
{
	return lookup_table_[index.internalId()].first;
}

quint32 ShaderGroupModel::getIndex(void* ptr) const
{
	auto find_iter = std::find_if(std::begin(lookup_table_), std::end(lookup_table_),
		[ptr](const std::vector<std::pair<uint32_t, void*>>::value_type& item)
	{
		return item.second == ptr;
	});

	int count = find_iter - lookup_table_.begin();

	return count;
}

shader_family* ShaderGroupModel::familyFromIndex(const QModelIndex& index) const
{
	if (index.isValid())
	{
		const auto& item = lookup_table_[index.internalId()];

		if (item.first == 0)
		{

			return static_cast<shader_family*>(item.second);
		}
	}

	return nullptr;
}

shader_family::shader_child* ShaderGroupModel::childFromIndex(const QModelIndex& index) const
{
	if (index.isValid())
	{
		const auto& item = lookup_table_[index.internalId()];

		if (item.first == 1)
		{

			return static_cast<shader_family::shader_child*>(item.second);
		}
	}

	return nullptr;
}
