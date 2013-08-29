
#include "shader_group_widget.h"

#include <QLabel>
#include <QSplitter>
#include <QToolBar>
#include <QTreeView>
#include <QBoxLayout>
#include <QPlainTextEdit>

#include "swganh/utilities.h"
#include "swganh/tre/tre_archive.h"

//#include "shader_group_model.h"
#include "shader_family_form.h"
#include "shader_child_form.h"
#include "dds_preview.h"

using swganh::tre::TreArchive;
using swganh::terrain::shader_family;
using swged::ShaderChildForm;
using swged::ShaderFamilyForm;
using swged::ShaderGroupWidget;
//using swged::ShaderGroupModel;
using swged::DDSPreview;

ShaderGroupWidget::ShaderGroupWidget(QWidget* parent)
	: QWidget(parent)
	, toolbar_(swganh::make_unique<QToolBar>())
	, family_tree_(swganh::make_unique<QTreeView>())
	, shader_preview_(swganh::make_unique<DDSPreview>())
{
	connect(family_tree_.get(), SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));

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

	//shader_preview_->setText("No shader selected");

	QSplitter* contents = new QSplitter(this);
	contents->addWidget(family_tree_.get());
	contents->addWidget(shader_preview_.get());

	QVBoxLayout* vbox = new QVBoxLayout();
	vbox->addWidget(toolbar_.get());
	vbox->addWidget(contents);

	setLayout(vbox);
}

ShaderGroupWidget::~ShaderGroupWidget()
{}

void ShaderGroupWidget::setShaderGroup(swganh::terrain::shader_group_t* shader_group)
{
	model_ = swganh::make_unique<ShaderGroupModel>(shader_group);

	family_tree_->setModel(model_.get());
	family_tree_->expandAll();

	console_->insertPlainText("Added shader group\n");
}

void ShaderGroupWidget::setConsole(QPlainTextEdit* console)
{
	console_ = console;
	shader_preview_->setConsole(console_);
}

void ShaderGroupWidget::setArchive(TreArchive* archive)
{
	archive_ = archive;
}

void ShaderGroupWidget::itemClicked(const QModelIndex& index)
{
	auto data = index.data(Qt::DisplayRole);

	console_->insertPlainText(QString::fromStdString("Selected ").append(data.toString()).append("\n"));

	QWidget* property_form = nullptr;

	if (index.parent().isValid())
	{
		console_->insertPlainText(QString::fromStdString("Loading preview for ").append(data.toString()).append("\n"));

		std::string resource_name = std::string("texture/").append(data.toString().toStdString()).append(".dds");
		shader_preview_->loadDDSFromBuffer(archive_->GetResource(resource_name));

		property_form = new ShaderChildForm(index.data(Qt::UserRole).value<shader_family::shader_child*>(), this);
	}
	else
	{
		property_form = new ShaderFamilyForm(index.data(Qt::UserRole).value<shader_family*>(), this);
	}

	emit propertiedItemSelected(property_form);
}

