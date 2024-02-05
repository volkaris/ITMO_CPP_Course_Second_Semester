#pragma once

#include "Mp3Frame.h"

Mp3Frame::Mp3Frame(std::string Name, std::string Value) : Name(Name), Value(Value) {
}

std::string Mp3Frame::get_value() {
	return Value;
}

std::string Mp3Frame::get_name() {
	return Name;
}
