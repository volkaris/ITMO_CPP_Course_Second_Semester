#pragma once
#include "InheritedClasses.h"


Identity_Info::Identity_Info(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};

inline std::string Identity_Info::get_info_about_frame_type() {
	return R"(Identity Information: Contains information about the identity of the audio file, including the artist, album, and track name.)";
}

Audio_Characteristics::Audio_Characteristics(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};


inline std::string Audio_Characteristics::get_info_about_frame_type() {
	return R"(Contains information about the musical content of the audio file, such as the genre and tempo.)";
}

Attached_Picture_and_Visual_Information::Attached_Picture_and_Visual_Information(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};


inline std::string Attached_Picture_and_Visual_Information::get_info_about_frame_type() {
	return R"(Contains information about images and videos related to the audio file.)";
}

Comments_and_Other_Information::Comments_and_Other_Information(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
}


inline std::string Comments_and_Other_Information::get_info_about_frame_type() {
	return R"(Contains information intended for private use, such as notes and comments.)";
}

Commercial_Information::Commercial_Information(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};


inline std::string Commercial_Information::get_info_about_frame_type() {
	return R"(Contains information about commercial aspects of the audio file, such as the price and availability.)";
}

Encryption_and_Security::Encryption_and_Security(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};


inline std::string Encryption_and_Security::get_info_about_frame_type() {
	return R"(Contains information about the encryption and security of the audio file, such as the encryption method and key.)";
}

Linked_Information::Linked_Information(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};


inline std::string Linked_Information::get_info_about_frame_type() {
	return R"(The Link group includes tags that provide information about a link to a related resource, such as a website or a file. These tags provide a way to add external or internal links within the MP3 file.)";
}

Miscellaneous::Miscellaneous(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};


inline std::string Miscellaneous::get_info_about_frame_type() {
	return R"(Miscellaneous Information: Contains information that doesn't fit into any other categories, such as websites and email addresses.)";
}

OfficialInfo::OfficialInfo(std::string Name, std::string Value) : Mp3Frame(Name, Value) {
};

inline std::string OfficialInfo::get_info_about_frame_type() {
	return R"(This frame contain some official info such as Official audio file webpage, Official artist/performer webpage etc.)";
}
