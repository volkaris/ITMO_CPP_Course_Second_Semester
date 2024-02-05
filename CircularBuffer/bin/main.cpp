#include <iostream>

using namespace std;

#include  "lib/static_buffer.h"
#include "lib/extented_buffer.h"

int main() {

	ExtendableBuffer<std::string> buffer(10);
	buffer.PushBack("first");
	buffer.PushBack("second");
	buffer.PushBack("third");
	buffer.Erase(buffer.begin(), buffer.begin() + 2);
	cout << buffer[0];
}