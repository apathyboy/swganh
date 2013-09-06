
#include "radial_group_widget.h"

#include <QLabel>
#include <QSplitter>
#include <QToolBar>
#include <QTreeView>
#include <QBoxLayout>
#include <QPlainTextEdit>

#include "swganh/tre/tre_archive.h"

#include "dds_preview.h"

using swganh::tre::TreArchive;
using swged::RadialGroupWidget;
using swged::DDSPreview;

RadialGroupWidget::RadialGroupWidget(QWidget* parent)
	: QWidget(parent)
	, toolbar_(swganh::make_unique<QToolBar>())
	, family_tree_(swganh::make_unique<QTreeView>())
	, shader_preview_(swganh::make_unique<DDSPreview>())
{
	family_tree_->setHeaderHidden(true);

	toolbar_ = swganh::make_unique<QToolBar>();
	toolbar_->addAction("add new");
	toolbar_->addAction("promote");
	toolbar_->addAction("demote");
	toolbar_->addAction("add child");
	toolbar_->addAction("find");
	toolbar_->addAction("verify");
	toolbar_->addAction("delete");
	toolbar_->addAction("delete unused");

	QSplitter* contents = new QSplitter(this);
	contents->addWidget(family_tree_.get());
	contents->addWidget(shader_preview_.get());

	QVBoxLayout* vbox = new QVBoxLayout();
	vbox->addWidget(toolbar_.get());
	vbox->addWidget(contents);

	setLayout(vbox);
}

RadialGroupWidget::~RadialGroupWidget()
{}

void RadialGroupWidget::setRadialGroup(swganh::terrain::radial_group_t* radial_group)
{
	model_ = swganh::make_unique<RadialGroupModel>(radial_group);

	family_tree_->setModel(model_.get());
	family_tree_->expandAll();

	console_->insertPlainText("Added shader group\n");
}

void RadialGroupWidget::setConsole(QPlainTextEdit* console)
{
	console_ = console;
	shader_preview_->setConsole(console_);
}

void RadialGroupWidget::setArchive(TreArchive* archive)
{
	archive_ = archive;
}

void RadialGroupWidget::itemClicked(const QModelIndex& index)
{
	auto data = index.data(Qt::DisplayRole);

	console_->insertPlainText(QString::fromStdString("Selected ").append(data.toString()).append("\n"));

	QWidget* property_form = nullptr;

	if (index.parent().isValid())
	{
		console_->insertPlainText(QString::fromStdString("Loading preview for ").append(data.toString()).append("\n"));

		std::string resource_name = std::string("texture/").append(data.toString().toStdString()).append(".dds");
		shader_preview_->loadDDSFromBuffer(archive_->GetResource(resource_name));
	}
}

