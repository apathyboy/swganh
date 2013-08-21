
#define STRICT

#include "dds.h"

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

#include "swganh/utilities.h"

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

std::ifstream open_dds_file(std::string filename);
std::unique_ptr<DDS_IMAGE_DATA> read_dds_image_data(std::string filename);

QImage swged::build_dds_thumbnail(const QString& file)
{
	auto dds_data = read_dds_image_data(file.toStdString());

	return QImage();
}


std::unique_ptr<DDS_IMAGE_DATA> read_dds_image_data(std::string filename)
{
	auto image_data = swganh::make_unique<DDS_IMAGE_DATA>();
	DDSURFACEDESC2 ddsd;
	int factor;
	int buffer_size;

	auto dds_stream = open_dds_file(filename);

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

std::ifstream open_dds_file(std::string filename)
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
