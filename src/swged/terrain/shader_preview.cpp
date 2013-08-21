
#include "shader_preview.h"

#include <iostream>
#include <QDebug>
#include <QPlainTextEdit>

#include "nv_dds/nv_dds.h"

using nv_dds::CDDSImage;
using swged::ShaderPreview;

std::string _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

static bool openGLSupportsExtension(const char *extension)
{
    // Adapted from http://www.opengl.org/resources/features/OGLextensions/

    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;


    // Check for illegal spaces in extension name
    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;


    extensions = glGetString(GL_EXTENSIONS);

    start = extensions;
    for (;;) {


        where = (GLubyte *) strstr((const char *) start, extension);


        if (!where)
            break;


        terminator = where + strlen(extension);


        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return true;


        start = terminator;
    }
    return false;
}

ShaderPreview::ShaderPreview(QWidget* parent)
	: QGLWidget(parent)
{
    qDebug() << "test";
}

void ShaderPreview::setConsole(QPlainTextEdit* console)
{
    console_ = console;
}

void ShaderPreview::setShader(const QString& shader_name)
{
    CDDSImage image;
    GLuint texobj;

    image.load("grss_long_darkgreen.dds");

    if (!image.is_valid())
    {
        console_->insertPlainText(QString::fromStdString("What the hoof\n"));
        return;
    }

    glGenTextures(1, &texobj);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texobj);

    glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, image.get_format(),
        image.get_width(), image.get_height(), 0, image.get_size(),
        image);

//    auto error = check_gl_error();
//    console_->insertPlainText(QString::fromStdString(error).append("\n"));
//    glEnable(GL_TEXTURE_2D);
//
//    makeCurrent();
//
//#if defined(QT_OPENGL_ES_2)
//    {
//        console_->insertPlainText(QString::fromStdString("What the hoof\n");
//        return;
//    }
//#endif
//
//    auto texture = bindTexture("grss_long_darkgreen.dds");
//
//    GLint width, height;
//    glBindTexture(GL_TEXTURE_2D, texture);
//    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
//    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
//
//    if (width == 0 || height == 0)
//    {
//        error = check_gl_error();
//        console_->insertPlainText(QString::fromStdString(error).append("\nSomething went wrong\n"));
//        return;
//    }
//
//    console_->insertPlainText("Something went right!\n");

}

std::string _check_gl_error(const char *file, int line) {
        GLenum err (glGetError());

                std::string error;
        while(err!=GL_NO_ERROR) {

                switch(err) {
                        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
                        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
                        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
                        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
                }

                std::cerr << "GL_" << error.c_str() <<" - "<<file<<":"<<line<<std::endl;
                err=glGetError();
        }
        return error;
}
