#pragma once

#include <fstream>
#include <iostream>
#include <string>

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