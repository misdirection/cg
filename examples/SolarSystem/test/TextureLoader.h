#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H
#include<string>
class TextureLoader
{
	public:
		static void loadImageToGLTexture(unsigned& texture_handle, std::string const& image_path, unsigned color_format, unsigned texture_unit);

	private:
		TextureLoader() {};
		~TextureLoader() {};
};
#endif