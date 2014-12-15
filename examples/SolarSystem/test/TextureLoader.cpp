#include "TextureLoader.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include<FreeImage.h>

void TextureLoader::loadImageToGLTexture(unsigned& texture_handle, std::string const& image_path, unsigned color_format, unsigned texture_unit)
{
	glGenTextures(1, &texture_handle);

	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	FIBITMAP* bitmap = FreeImage_Load(
		FreeImage_GetFileType(image_path.c_str(), 0),
		image_path.c_str());

	FIBITMAP * pImage = FreeImage_ConvertTo24Bits(bitmap);
	int nWidth = FreeImage_GetWidth(pImage);
	int nHeight = FreeImage_GetHeight(pImage);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, nWidth, nHeight,
		0, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

	FreeImage_Unload(pImage);

}