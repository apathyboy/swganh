
#pragma once

#include <QtOpenGL>
#include <QtOpenGLExtensions>
#include <QGLWidget>

class QPlainTextEdit;

namespace swged {

	class ShaderPreview : public QGLWidget
	{
		Q_OBJECT

	public:
		ShaderPreview(QWidget* parent = nullptr);

        void setConsole(QPlainTextEdit* console);

        void setShader(const QString& shader_name);

    private:
        QPlainTextEdit* console_;
	};

}
