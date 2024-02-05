#pragma once
#include "Mp3Frame.h"

class Identity_Info : public Mp3Frame {
public:
	Identity_Info(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Audio_Characteristics : public Mp3Frame {
public:
	Audio_Characteristics(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Attached_Picture_and_Visual_Information : public Mp3Frame {
public:
	Attached_Picture_and_Visual_Information(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Comments_and_Other_Information : public Mp3Frame {
public:
	Comments_and_Other_Information(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Commercial_Information : public Mp3Frame {
public:
	Commercial_Information(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Encryption_and_Security : public Mp3Frame {
public:
	Encryption_and_Security(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Linked_Information : public Mp3Frame {
public:
	Linked_Information(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class Miscellaneous : public Mp3Frame {
public:
	Miscellaneous(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};

class OfficialInfo : public Mp3Frame {
public:
	OfficialInfo(std::string Name, std::string Value);
	std::string get_info_about_frame_type() override;
};
