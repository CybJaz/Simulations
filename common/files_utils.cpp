#include "files_utils.h"

std::string load_txt_file(const std::string& file_name)
{
	std::ifstream file;
	file.open((file_name).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << file_name << std::endl;
	}

	return output;
}

BitMap::BitMap(const char * filename)
{
	try
	{
		FILE * file = fopen(filename, "rb");
		if (!file)
		{
			printf("%s could not be opened. Are you in the right directory ? \n", filename);
			getchar();
			// Throw exception?
		}

		// Read the header, i.e. the 54 first bytes

		// If less than 54 bytes are read, problem
		if (fread(header, 1, 54, file) != 54){
			printf("Not a correct BMP file\n");
			//return 0;
		}
		// A BMP files always begins with "BM"
		if (header[0] != 'B' || header[1] != 'M'){
			printf("Not a correct BMP file\n");
			//return 0;
		}
		// Make sure this is a 24bpp file
		if (*(int*)&(header[0x1E]) != 0)
			printf("Not a correct BMP file\n");
		if (*(int*)&(header[0x1C]) != 24)
			printf("Not a correct BMP file\n");

		// Read the information about the image
		data_pos = *(int*)&(header[0x0A]);
		image_size = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		// Some BMP files are misformatted, guess missing information
		if (image_size == 0)
			image_size = width * height * 3; // 3 : one byte for each Red, Green and Blue component
		if (data_pos == 0)      data_pos = 54; // The BMP header is done that way

		// Create a buffer
		data = new unsigned char[image_size];

		// Read the actual data from the file into the buffer
		fread(data, 1, image_size, file);

		// Everything is in memory now, the file wan be closed
		fclose(file);
	}
	catch (std::exception & e)
	{

	}
}