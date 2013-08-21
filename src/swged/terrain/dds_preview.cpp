
#include "dds_preview.h"

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGl/gl.h>
#include <OpenGl/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef WIN32
#include "glext.h"
#include <ddraw.h>
#else
#include <GL/glext.h>
#endif

#include <fstream>

#include <QPlainTextEdit>

#include "swganh/byte_buffer.h"
#include "swganh/utilities.h"

using swganh::ByteBuffer;
using swged::DDSPreview;

// ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;

struct DDS_IMAGE_DATA
{
	GLsizei  width;
	GLsizei  height;
	GLint    components;
	GLenum   format;
	int      numMipMaps;
	std::vector<GLubyte> pixels;
};

struct Vertex
{
	float tu, tv;
	float x, y, z;
};

Vertex g_quadVertices [] =
{
	{ 0.0f, 0.0f, -1.0f, -1.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f, -1.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, -1.0f, 1.0f, 0.0f }
};

void convertFromGLImage(QImage &img, int w, int h, bool alpha_format, bool include_alpha);
QImage qt_gl_read_texture(const QSize &size, bool alpha_format, bool include_alpha);

namespace swged {
	struct DDSPreview::DDSPreviewImpl
	{
		DDSPreviewImpl();
		~DDSPreviewImpl();

		void paint();

		ByteBuffer readDDSToBuffer(std::string filename);

		std::unique_ptr<DDS_IMAGE_DATA> readDDSImageData(ByteBuffer texture_buffer);
		void initGl();
		void loadDDSData(ByteBuffer texture_buffer);

		QPlainTextEdit* console;
		QImage thumbnail;

		float fDistance;
		float fSpinX;
		float fSpinY;
		Vertex quadVertices[4];

		std::unique_ptr<DDS_IMAGE_DATA> current_image_data;
		GLuint compressedTextureID;
		HWND hWnd;
		HDC hDC;
		HGLRC hRC;
		DDSPreview* widget;
	};
}

DDSPreview::DDSPreview(QWidget* parent)
	: QWidget(parent)
	, impl_(swganh::make_unique<DDSPreviewImpl>())
{
	impl_->widget = this;
	impl_->initGl();

	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);

	setAutoFillBackground(true);
}

DDSPreview::~DDSPreview()
{}

void DDSPreview::setConsole(QPlainTextEdit* console)
{
	impl_->console = console;
}

void DDSPreview::loadDDSFromFile(QString filename)
{
	try
	{
		loadDDSFromBuffer(impl_->readDDSToBuffer(filename.toStdString()));
	}
	catch (std::exception& e)
	{
		impl_->console->insertPlainText(QString::fromStdString(e.what()).append("\n"));
		return;
	}
}

void DDSPreview::loadDDSFromBuffer(ByteBuffer buffer)
{
	try
	{
		impl_->loadDDSData(std::move(buffer));
	}
	catch (std::exception& e)
	{
		impl_->console->insertPlainText(QString::fromStdString(e.what()).append("\n"));
		return;
	}

	repaint();
}

QImage DDSPreview::getDDSPreview() const
{
	return impl_->thumbnail;
}

void DDSPreview::paintEvent(QPaintEvent *event)
{
	impl_->paint();
}

DDSPreview::DDSPreviewImpl::DDSPreviewImpl()
	: compressedTextureID(-1)
{
	fDistance = -3.0f;
	fSpinX = 0.0f;
	fSpinY = 0.0f;
}

DDSPreview::DDSPreviewImpl::~DDSPreviewImpl()
{
	glDeleteTextures(1, &compressedTextureID);

	if (hRC != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		hRC = NULL;
	}

	if (hDC != NULL)
	{
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
	}
}

void DDSPreview::DDSPreviewImpl::paint()
{
	wglMakeCurrent(hDC, hRC);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (compressedTextureID != -1)
	{
		glPushMatrix();
		glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, -1.0);
		glTexCoord2d(1.0, 0.0); glVertex2d(+1.0, -1.0);
		glTexCoord2d(1.0, 1.0); glVertex2d(+1.0, +1.0);
		glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, +1.0);
		glEnd();
		glPopMatrix();
	}

	SwapBuffers(hDC);
}

ByteBuffer DDSPreview::DDSPreviewImpl::readDDSToBuffer(std::string filename)
{
	std::ifstream in(filename, std::ios::binary);

	if (!in.is_open())
	{
		throw std::runtime_error("Invalid filename given: " + filename);
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

	in.close();

	return swganh::ByteBuffer(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());
}

std::unique_ptr<DDS_IMAGE_DATA> DDSPreview::DDSPreviewImpl::readDDSImageData(ByteBuffer texture_buffer)
{
	if (strncmp(reinterpret_cast<char*>(texture_buffer.data()), "DDS ", 4) != 0)
	{
		throw std::runtime_error("Requested resource is not a DDS file.");
	}

	texture_buffer.read_position_delta(4);

	auto image_data = swganh::make_unique<DDS_IMAGE_DATA>();
	DDSURFACEDESC2 ddsd;
	int factor;
	int buffer_size;
	
	ddsd = texture_buffer.read<DDSURFACEDESC2>();
	
	switch (ddsd.ddpfPixelFormat.dwFourCC)
	{
	case FOURCC_DXT1:
		// DXT1's compression ratio is 8:1
		image_data->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		factor = 2;
		break;
	
	case FOURCC_DXT3:
		// DXT3's compression ratio is 4:1
		image_data->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		factor = 4;
		break;
	
	case FOURCC_DXT5:
		// DXT5's compression ratio is 4:1
		image_data->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		factor = 4;
		break;
	
	default:
		throw std::runtime_error("DDS file is not compressed using one of the DXT1, DXT3, or DXT5 formats");
	}
	
	if (ddsd.dwLinearSize == 0)
	{
		throw std::runtime_error("dwLinearSize is 0");
	}
	
	if (ddsd.dwMipMapCount > 1)
		buffer_size = ddsd.dwLinearSize * factor;
	else
		buffer_size = ddsd.dwLinearSize;
	
	image_data->pixels.resize(buffer_size * sizeof(unsigned char));
	image_data->pixels.insert(image_data->pixels.begin(), 
		texture_buffer.data() + texture_buffer.read_position(),
		texture_buffer.data() + texture_buffer.size());
	
	image_data->width = ddsd.dwWidth;
	image_data->height = ddsd.dwHeight;
	image_data->numMipMaps = ddsd.dwMipMapCount;
	
	if (ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1)
		image_data->components = 3;
	else
		image_data->components = 4;
	
	return image_data;
}

void DDSPreview::DDSPreviewImpl::initGl()
{
	GLuint PixelFormat;
	hWnd = (HWND)widget->winId();
	hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, PixelFormat, &pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	glClearColor( 0.35f, 0.53f, 0.7f, 1.0f );
	glEnable(GL_TEXTURE_2D);
	
	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity();
	//gluPerspective( 45.0f, 256.0f / 256.0f, 0.1f, 100.0f);

	char *ext = (char*) glGetString(GL_EXTENSIONS);

	if (strstr(ext, "ARB_texture_compression") == NULL)
	{
		throw std::runtime_error("ARB_texture_compression extension was not found");
	}
	else
	{
		glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) wglGetProcAddress("glCompressedTexImage2DARB");

		if (!glCompressedTexImage2DARB)
		{
			throw std::runtime_error("One or more ARB_texture_compression functions were not found");
		}
	}
}

void DDSPreview::DDSPreviewImpl::loadDDSData(ByteBuffer texture_buffer)
{
	auto dds_data = readDDSImageData(std::move(texture_buffer));

	if (!dds_data)
	{
		throw std::runtime_error("Error reading DDS image data");
	}

	int nHeight = dds_data->height;
	int nWidth = dds_data->width;
	int nNumMipMaps = dds_data->numMipMaps;

	int nBlockSize;

	if (dds_data->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
		nBlockSize = 8;
	else
		nBlockSize = 16;

	glGenTextures(1, &compressedTextureID);
	glBindTexture(GL_TEXTURE_2D, compressedTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nSize;
	int nOffset = 0;

	// Load the mip-map levels

	for (int i = 0; i < nNumMipMaps; ++i)
	{
		if (nWidth == 0) nWidth = 1;
		if (nHeight == 0) nHeight = 1;

		nSize = ((nWidth + 3) / 4) * ((nHeight + 3) / 4) * nBlockSize;

		glCompressedTexImage2DARB(GL_TEXTURE_2D,
			i,
			dds_data->format,
			nWidth,
			nHeight,
			0,
			nSize,
			dds_data->pixels.data() + nOffset);

		nOffset += nSize;

		// Half the image size for the next mip-map level...
		nWidth = (nWidth / 2);
		nHeight = (nHeight / 2);
	}
}

QImage qt_gl_read_texture(const QSize &size, bool alpha_format, bool include_alpha)
{
	QImage img(size, alpha_format ? QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32);

	int w = size.width();
	int h = size.height();

	//### glGetTexImage not in GL ES 2.0, need to do something else here!
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

	convertFromGLImage(img, w, h, alpha_format, include_alpha);

	return img;
}

void convertFromGLImage(QImage &img, int w, int h, bool alpha_format, bool include_alpha)
{
	if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
		// OpenGL gives RGBA; Qt wants ARGB
		uint *p = (uint*) img.bits();
		uint *end = p + w*h;

		if (alpha_format && include_alpha) {
			while (p < end) {
				uint a = *p << 24;
				*p = (*p >> 8) | a;
				p++;
			}
		}
		else {
			// This is an old legacy fix for PowerPC based Macs, which
			// we shouldn't remove
			while (p < end) {
				*p = 0xff000000 | (*p >> 8);
				++p;
			}
		}
	}
	else {
		// OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
		for (int y = 0; y < h; y++) {
			uint *q = (uint*) img.scanLine(y);
			for (int x = 0; x < w; ++x) {
				const uint pixel = *q;

				if (alpha_format && include_alpha) {
					*q = ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff)
						| (pixel & 0xff00ff00);
				}
				else {
					*q = 0xff000000 | ((pixel << 16) & 0xff0000)
						| ((pixel >> 16) & 0xff) | (pixel & 0x00ff00);
				}

				q++;
			}
		}
	}

	img = img.mirrored();
}

