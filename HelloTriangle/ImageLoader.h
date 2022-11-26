#pragma once
#include "pch.h"

//Wrapper Class for stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class
	ImageLoadRaw
{
public:
	ImageLoadRaw() {};
	ImageLoadRaw(dxh::ImageData& target, const char* filepath);
	~ImageLoadRaw() {};
	bool ImageFromFile(dxh::ImageData& target, const char* filepath);

};

inline 
ImageLoadRaw::ImageLoadRaw(dxh::ImageData& target, const char* filepath) 
{
	ImageFromFile(target, filepath);
}

inline
bool ImageLoadRaw::ImageFromFile(dxh::ImageData &target, const char* filepath)
{
	int width, height, channels = 4;

	unsigned char* img = stbi_load(filepath, &width, &height, &channels, 4);
	if (img == NULL)
	{
		util::ErrorMessageBox("could not load image from file \"" + std::string(filepath) + "\".");
		if (img) stbi_image_free(img);
		return false;
	}

	//forcing 4 channels
	channels = 4;
	//n would be the total size of the image
	std::vector<unsigned char> temp(img, img + (width * height * channels)); 

	target.channels = channels; //force 4 channels
	target.height = height;
	target.width = width;
	target.data = temp;

	//free the data
	stbi_image_free(img); 

	return true;
}