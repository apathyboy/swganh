
#include "shader_group_widget.h"

#include <QLabel>
#include <QSplitter>
#include <QToolBar>
#include <QTreeView>
#include <QBoxLayout>
#include <QPlainTextEdit>

#include "swganh/utilities.h"

#include "shader_group_model.h"
#include "shader_preview.h"

using swged::ShaderGroupWidget;
using swged::ShaderGroupModel;
using swged::ShaderPreview;

ShaderGroupWidget::ShaderGroupWidget(QWidget* parent)
	: QWidget(parent)
	, toolbar_(swganh::make_unique<QToolBar>())
	, family_tree_(swganh::make_unique<QTreeView>())
	, shader_preview_(swganh::make_unique<ShaderPreview>())
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

void ShaderGroupWidget::itemClicked(const QModelIndex& index)
{
	auto data = index.data(Qt::DisplayRole);

	QString msg("Selected ");
	msg.append(data.toString());
	msg.append("\n");

	console_->insertPlainText(msg);
	shader_preview_->setShader(data.toString());
}

