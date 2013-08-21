
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

#include "swganh/utilities.h"

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

namespace swged {
	struct DDSPreview::DDSPreviewImpl
	{
		std::ifstream openDDSFile(std::string filename);
		std::unique_ptr<DDS_IMAGE_DATA> readDDSImageData(std::string filename);
		void initGl();
		QImage buildDDSThumbnail(const QString& file);

		QPlainTextEdit* console;
		QImage thumbnail;

		DDSPreview* widget;
	};
}

DDSPreview::DDSPreview(QWidget* parent)
	: QWidget(parent)
	, impl_(swganh::make_unique<DDSPreviewImpl>())
{
	impl_->widget = this;
	impl_->initGl();
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
		impl_->thumbnail = impl_->buildDDSThumbnail("grss_long_darkgreen.dds");
	}
	catch (std::exception& e)
	{
		impl_->console->insertPlainText(QString::fromStdString(e.what()).append("\n"));
		return;
	}

	impl_->console->insertPlainText(QString::fromStdString("So far, so good\n"));
}


std::ifstream DDSPreview::DDSPreviewImpl::openDDSFile(std::string filename)
{
	char filecode[4];

	std::ifstream in(filename.c_str(), std::ios::binary);

	if (!in.is_open())
	{
		throw std::runtime_error("Unable to open requested DDS resource file");
	}

	in.read(filecode, sizeof(filecode));

	if (strncmp(filecode, "DDS ", sizeof(filecode)) != 0)
	{
		throw std::runtime_error("Requested resource is not a DDS file.");
	}

	return in;
}

std::unique_ptr<DDS_IMAGE_DATA> DDSPreview::DDSPreviewImpl::readDDSImageData(std::string filename)
{
	auto image_data = swganh::make_unique<DDS_IMAGE_DATA>();
	DDSURFACEDESC2 ddsd;
	int factor;
	int buffer_size;

	auto dds_stream = openDDSFile(filename);

	dds_stream.read(reinterpret_cast<char*>(&ddsd), sizeof(ddsd));

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

	image_data->pixels.resize(buffer_size * sizeof(unsigned char) );

	dds_stream.read(reinterpret_cast<char*>(image_data->pixels.data()), buffer_size);
	dds_stream.close();

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
{}

QImage DDSPreview::DDSPreviewImpl::buildDDSThumbnail(const QString& file)
{
	auto dds_data = readDDSImageData(file.toStdString());
	GLuint g_compressedTextureID = -1;

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

	glGenTextures(1, &g_compressedTextureID);
	glBindTexture(GL_TEXTURE_2D, g_compressedTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return QImage();
}
