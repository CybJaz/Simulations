#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

/**
	Loader based on: 
*/
struct BitMap
{
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int data_pos;
	unsigned int image_size;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	BitMap(const char * filename);
	~BitMap() { delete[] data; };
};

std::string load_txt_file(const std::string& file_name);