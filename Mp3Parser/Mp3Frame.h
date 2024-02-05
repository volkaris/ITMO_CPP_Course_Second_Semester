#pragma once

#include <string>

class Mp3Frame {
	std::string Name;
	std::string Value;
public:
	Mp3Frame(std::string Name, std::string Value);
	virtual ~Mp3Frame() {
	};
	std::string get_value();
	std::string get_name();
	virtual std::string get_info_about_frame_type() = 0;
};
