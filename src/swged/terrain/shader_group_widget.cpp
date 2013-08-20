
#include "shader_group_widget.h"

#include <QLabel>
#include <QSplitter>
#include <QToolBar>
#include <QTreeView>
#include <QBoxLayout>
#include <QGLWidget>
#include <QGLPixelBuffer>
#include "qopenglfunctions_3_3_core.h"


#include "swganh/utilities.h"

#include "shader_group_model.h"

using swged::ShaderGroupWidget;
using swged::ShaderGroupModel;

ShaderGroupWidget::ShaderGroupWidget(QWidget* parent)
	: QWidget(parent)
	, toolbar_(swganh::make_unique<QToolBar>())
	, family_tree_(swganh::make_unique<QTreeView>())
	, shader_preview_(swganh::make_unique<QGLWidget>())
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
	model_ = std::make_unique<ShaderGroupModel>(shader_group);

	family_tree_->setModel(model_.get());
	family_tree_->expandAll();
}

void ShaderGroupWidget::itemClicked(const QModelIndex& index)
{
	auto data = index.data(Qt::DisplayRole);

	readDDSFile("grss_long_darkgreen.dds");

	//shader_preview_->setText(data.toString());
}


QImage ShaderGroupWidget::readDDSFile(const QString& filename)
{
	shader_preview_->makeCurrent();
	
	GLuint texture = shader_preview_->bindTexture(filename);
	if (!texture)
		return QImage();

	// Determine the size of the DDS image
	//GLint width, height;
	glBindTexture(GL_TEXTURE_2D, texture);
	shader_preview_->drawTexture(QPoint(100.f, 100.f), texture, GL_TEXTURE_2D);

	//glFuncs.glGetglGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	//glFuncs.glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	//if (width == 0 || height == 0)
	//	return QImage();

	QGLPixelBuffer pbuffer(QSize(100, 100), shader_preview_->format(), shader_preview_.get());
	if (!pbuffer.makeCurrent())
		return QImage();

	pbuffer.drawTexture(QRectF(-1, -1, 2, 2), texture);
	return pbuffer.toImage();

}
