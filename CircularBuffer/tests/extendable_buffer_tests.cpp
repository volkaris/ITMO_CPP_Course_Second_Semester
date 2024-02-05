#include <gtest/gtest.h>
#include "lib/Extented_buffer.h"

TEST(VariousFunctionsTests, PushFront) {

	ExtendableBuffer<std::string> buffer;
	buffer.PushFront("last");
	buffer.PushFront("third");
	buffer.PushFront("second");
	buffer.PushFront("first");
	ASSERT_EQ(buffer[0], "first");
	ASSERT_EQ(buffer[1], "second");
	ASSERT_EQ(buffer[2], "third");
	ASSERT_EQ(buffer[3], "last");
}

TEST(VariousFunctionsTests, PushBack) {

	ExtendableBuffer<std::string> buffer;
	buffer.PushBack("first");
	buffer.PushBack("second");
	buffer.PushBack("third");
	buffer.PushBack("last");
	ASSERT_EQ(buffer[0], "first");
	ASSERT_EQ(buffer[1], "second");
	ASSERT_EQ(buffer[2], "third");
	ASSERT_EQ(buffer[3], "last");
}

TEST(InsertTests, ManyInserts) {
	ExtendableBuffer<std::string> buffer;
	buffer.PushBack("first");
	buffer.PushBack("second");
	buffer.PushBack("third");
	buffer.PushBack("last");
	buffer.Insert(buffer.begin() + 1, 3, "i'm inserted");
	ASSERT_EQ(buffer[0], "first");
	ASSERT_EQ(buffer[1], "i'm inserted");
	ASSERT_EQ(buffer[3], "i'm inserted");
	ASSERT_EQ(buffer[4], "second");
	ASSERT_EQ(*(buffer.end() - 1), "last");
}
TEST(InsertTests, SingleInsert) {
	ExtendableBuffer<std::string> buffer;
	buffer.PushBack("first");
	buffer.Insert(buffer.end() - 1, "i'm inserted by once insert");
	ASSERT_EQ(*(buffer.end() - 2), "i'm inserted by once insert");
}
TEST(InsertTests, InsertWithIterators) {
	ExtendableBuffer<std::string> buffer;
	buffer.PushBack("first");
	buffer.PushBack("second");
	ExtendableBuffer<std::string> second_buffer;
	second_buffer.Insert(second_buffer.begin(), buffer.begin(), buffer.end());
	ASSERT_EQ(buffer, second_buffer);
}
TEST(InsertTests, InsertWithInitializerList) {
	ExtendableBuffer<std::string> buffer;
	buffer.Insert(buffer.begin(),{ "hello","there" });
	ASSERT_EQ(buffer[0], "hello");
	ASSERT_EQ(buffer[1], "there");
}

TEST(EraseTests, ManyErases) {
	ExtendableBuffer<std::string> buffer;
	buffer.PushBack("first");
	buffer.PushBack("second");
	buffer.PushBack("third");
	buffer.Erase(buffer.begin(), buffer.begin() + 2);
	ASSERT_EQ(buffer[0], "third");

}
TEST(EraseTests, SingleErase) {
	ExtendableBuffer<std::string> buffer;
	buffer.PushBack("first");
	buffer.PushBack("second");
	buffer.PushBack("third");
	buffer.Erase(buffer.begin());
	ASSERT_EQ(buffer[0], "second");
	ASSERT_EQ(buffer[1], "third");
}