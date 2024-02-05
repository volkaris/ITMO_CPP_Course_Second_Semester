#pragma once

#include "GlobalVariables.h"
#include "InheritedClasses.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

inline std::vector<Mp3Frame*> SplitIntoClasses(const std::map<std::string, std::string>& Mp3Map) {
	std::vector<Mp3Frame*> result;
	for (auto& [key, value] : Mp3Map) {
		if (Identity_Info.contains(key)) {
			result.push_back(new class Identity_Info(key, value));
		} else if (Audio_Characteristics.contains(key)) {
			result.push_back(new class Audio_Characteristics(key, value));
		} else if (Attached_Picture_and_Visual_Information.contains(key)) {
			result.push_back(new class Attached_Picture_and_Visual_Information(key, value));
		} else if (Comments_and_Other_Information.contains(key)) {
			result.push_back(new class Comments_and_Other_Information(key, value));
		} else if (Commercial_Information.contains(key)) {
			result.push_back(new class Commercial_Information(key, value));
		} else if (Encryption_and_Security.contains(key)) {
			result.push_back(new class Encryption_and_Security(key, value));
		} else if (Linked_Information.contains(key)) {
			result.push_back(new class Linked_Information(key, value));
		} else if (Miscellaneous.contains(key)) {
			result.push_back(new class Miscellaneous(key, value));
		} else if (OfficialInfo.contains(key)) {
			result.push_back(new class OfficialInfo(key, value));
		}
	}

	return result;
}


inline bool isValid(char c) {
	return ((c >= '0') and (c <= '9')) or ((c >= 'A') and (c <= 'Z')) or ((c >= 'a') and (c <= 'z')) or (c == ' ') or ((c >= 33) and (c <= 125)) or (c == '\0');
}

inline int CalculateTagSize(std::string buffer) {
	return (buffer[6] & 0x7f) << 21 | (buffer[7] & 0x7f) << 14 | (buffer[8] & 0x7f) << 7 | (buffer[9] & 0x7f);
}

inline std::string GetTags(const std::string filename) {
	std::ifstream file(filename, std::ios::binary);
	std::string buffer(10, 0);
	file.read(buffer.data(), 10);
	tag_size = CalculateTagSize(buffer);
	buffer.resize(tag_size);
	file.read(buffer.data(), tag_size);
	file.seekg(0, std::ios::beg);
	std::string str;
	for (const auto& character : buffer) {
		if (isValid(character)) {
			if (character == '\0' or character == ' ') {
				str += ' ';
			} else {
				str += character;
			}
		}
	}

	return str;
}



inline std::vector<std::string> ParseTheTag(std::string& tag) {
	for (auto& current_tag : frameTags) {
		if (tag.find(current_tag) != std::string::npos) {
			const size_t where_To_add = tag.find(current_tag);
			std::string currentTAG = tag.substr(tag.find(current_tag), 4);
			tag[where_To_add] = '\n';
			tag[where_To_add + 5] = '=';
		}
	}
	std::vector<std::string> splitted_result;
	std::stringstream ss(tag);
	std::string token;

	while (std::getline(ss, token, '\n')) {
		splitted_result.push_back(token);
	}
	//splitted_result.erase(splitted_result.begin());

	return splitted_result;
}

inline void CorrectKeyMap(std::map<std::string, std::string>& mp3_map) {
	for (auto& current_tag : frameTags) {
		std::string currentTag;
		currentTag += current_tag[1];
		currentTag += current_tag[2];
		currentTag += current_tag[3];
		if (auto it = mp3_map.find(currentTag); it != mp3_map.end()) {
			auto s = it->second;
			mp3_map.erase(it);
			mp3_map.insert({current_tag, s});
		}
	}
}

inline void FillMap(std::map<std::string, std::string>& mp3_map, const std::vector<std::string>& splitted_result) {
	for (size_t i = 1; i < splitted_result.size(); i++) {
		std::string key = splitted_result[i].substr(0, splitted_result[i].find("="));
		erase(key, ' ');
		std::string value = splitted_result[i].substr(splitted_result[i].find("=") + 1, splitted_result[i].length());
		value = value.substr(value.find_first_not_of(" "), value.find_last_not_of(" "));
		mp3_map[key] = value;
	}

	CorrectKeyMap(mp3_map);
}

inline std::vector<std::pair<std::string, std::string>> SplitArgvAndGetFileName(const int argc, char* argv[], std::string& filename) {
	std::string new_argv;
	for (size_t i = 1; i < argc; i++) {
		new_argv += argv[i];
		if (i + 1 < argc) {
			new_argv += " ";
		}
	}
	std::vector<std::pair<std::string, std::string>> splitted_result;
	std::stringstream ss(new_argv);
	std::string token;
	while (getline(ss, token, ' ')) {
		std::string value = token.substr(0, token.find('='));
		std::string key = token.substr(token.find('=') + 1, token.length());
		splitted_result.push_back(make_pair(value, key));
	}
	if (splitted_result[0].first == "--file" or splitted_result[0].first == "-f") {
		filename = splitted_result[0].second;
	} else {
		throw std::invalid_argument("First argument must be name of mp3 file");
	}

	return splitted_result;
}


void ParceArgv(std::vector<std::pair<std::string, std::string>> splitted_result, const std::vector<Mp3Frame*> Classes) {
	for (int i = 1; i < splitted_result.size(); i++) {
		if (splitted_result[i].first == "--show" or splitted_result[i].first == "-s") {
			if (splitted_result[i].second == "all") {
				for (const auto j : Classes) {
					std::cout << j->get_name() << "=" << j->get_value() << "\n";
				}
			} else {
				for (const auto j : Classes) {
					if (splitted_result[i].second == j->get_name()) {
						std::cout << j->get_name() << "=" << j->get_value() << "\n";
						break;
					}
				}
			}
		} else if (splitted_result[i].first == "--description" or splitted_result[i].first == "-d") {
			if (splitted_result[i].second == "all") {
				for (const auto j : Classes) {
					std::cout << j->get_name() << "   " << j->get_info_about_frame_type() << "\n";
				}
			} else {
				for (const auto j : Classes) {
					if (splitted_result[i].second == j->get_name()) {
						std::cout << j->get_name() << "  " << j->get_info_about_frame_type() << "\n";
						break;
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
	std::string file_name;
	auto splitted_result = SplitArgvAndGetFileName(argc, argv, file_name);

	std::string str = GetTags(file_name);

	std::vector<std::string> splitted_result1 = ParseTheTag(str);

	std::map<std::string, std::string> Mp3Map;

	FillMap(Mp3Map, splitted_result1);

	std::vector<Mp3Frame*> classes = SplitIntoClasses(Mp3Map);

	//auto InstructionsToUser = R"( Firstly you need to type name of mp3 file to do this type --file=name.mp3 or -f=name.mp3.You can see name of every parameter and this value  with command --show=all or -s=all.If you want to see any specific parameter then --show=name_of_parameter or -s=name_of_parameter.
	//Also you can request small description of all or one parameter. To do this for all parameters type --description=all or -d=all.For exactly one parameter write --description=name_of_parameter or -d=name_of_parameter.)";
	ParceArgv(splitted_result, classes);
	for (const auto i : classes) {
		delete i;
	}
}
