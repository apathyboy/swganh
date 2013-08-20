
#pragma once

#include <QGLWidget>

namespace swged {

	class ShaderPreview : public QGLWidget
	{
		Q_OBJECT

	public:
		ShaderPreview(QWidget* parent = nullptr);

		void paintEvent(QPaintEvent*);
	};

}
