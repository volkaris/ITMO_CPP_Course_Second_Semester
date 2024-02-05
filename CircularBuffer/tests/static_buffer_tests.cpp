#include <lib/StaticBuffer.h>
#include <gtest/gtest.h>


TEST(VariousConstructorsTests, CopyConstructible) {
	Buffer buffer = { "hello","hi","text"};
	Buffer b(buffer);
	ASSERT_EQ(buffer, b);
	
	}

TEST(VariousConstructorsTests, SizeConstructible) {
	Buffer<std::string> buf(4);
	buf.PushBack("hello1");
	buf.PushBack("hello2");
	buf.PushBack("hello3");
	buf.PushBack("hello4");
	ASSERT_EQ(buf[0], "hello1");
	buf.PushBack("hello5");
	buf.PushBack("hello6");
	buf.PushBack("hello7");
	buf.PushBack("hello8");
	ASSERT_EQ(buf[0], "hello5");
	ASSERT_EQ(buf[1], "hello6");
	ASSERT_EQ(buf[2], "hello7");
	ASSERT_EQ(buf[3], "hello8");
}


TEST(VariousConstructorsTests, EmptyBuffer) {
		Buffer<int> buffer(0);
		ASSERT_EQ(buffer.begin(), buffer.end());
		ASSERT_EQ(std::distance(buffer.begin(), buffer.end()), 0);
		ASSERT_EQ(buffer.Size(), 0);
		ASSERT_THROW(buffer.Front(), std::out_of_range);
		ASSERT_THROW(buffer.Back(), std::out_of_range);
	}

	TEST(VariousConstructorsTests, ValueConstructible) {
		Buffer<std::string> buffer(100, "hello it's me");
		for (auto i = 0; i < buffer.Size(); i++) {
			ASSERT_EQ(buffer[i], "hello it's me");
		}
		for (auto i = 0; i < 50; i++) {
			buffer.PopBack();
		}
		ASSERT_EQ(buffer.Size(), 50);
	}


	TEST(VariousConstructorsTests, InitializerListConstructible) {
		Buffer<std::string> buffer({ "I","was","wondering","if","after","all","these" });
		ASSERT_EQ(buffer.Size(), 7);
		ASSERT_EQ(std::distance(buffer.begin(), buffer.end()), 7);
		ASSERT_EQ(buffer[0], "I");
		ASSERT_EQ(buffer[buffer.Size() - 1], "these");
	}



	TEST(OperatorEqualTests, InitializerListEquatable) {
		Buffer<int> buffer(4);
		buffer = { 100, 200, 300, 400 };
		ASSERT_EQ(buffer[0], 100);
		ASSERT_EQ(buffer[3], 400);
	}

	TEST(VariousConstructorsTests, IteratorConstructible) {
		Buffer<std::string> buffer({ "years","you'd","like","to","meet","to","go" });
		Buffer copied(buffer.begin(), buffer.end());
		ASSERT_EQ(buffer, copied);
	}



	TEST(OperatorEqualTests, AssignmentOperatorable) {
		Buffer<double> buffer({ 3.14,2.17,1.2113211323232131231232132131231231312332,3.124244232 });
		Buffer copied = buffer;
		ASSERT_EQ(buffer, copied);
		ASSERT_DOUBLE_EQ(copied[2], 1.2113211323232131231232132131231231312332);
	}



	TEST(MainFunctionsTests, PushBack) {
		Buffer<int>buffer(4);

		buffer.PushBack(1);
		buffer.PushBack(2);
		buffer.PushBack(3);
		buffer.PushBack(4);

		buffer.PushBack(100);
		ASSERT_EQ(buffer[0], 100);

		buffer.PushBack(200);
		ASSERT_EQ(buffer[1], 200);

		buffer.PushBack(300);
		ASSERT_EQ(buffer[2], 300);

		buffer.PushBack(400);
		ASSERT_EQ(buffer[3], 400);

		//1 2 3 4 -> 100 200 300 400
	}


	TEST(MainFunctionsTests, PopBack) {
		Buffer<double> buffer = { 1.56,2.14,3.12124,9.1000 };
		buffer.PopBack();
		ASSERT_EQ(buffer.Size(), 3);
		ASSERT_EQ(buffer[2], 3.12124);
		buffer.PopBack();
		buffer.PopBack();
		buffer.PopBack();
		buffer.PushBack(1.223);
		ASSERT_EQ(buffer[0], 1.223);
		buffer.PopBack();
		ASSERT_THROW(buffer.PopBack(), std::out_of_range);
	}


	TEST(MainFunctionsTests, PushFront) {
		Buffer<int> buffer(4);
		buffer.PushFront(400);
		buffer.PushFront(500);
		buffer.PushFront(600);
		buffer.PushFront(700);
		ASSERT_EQ(buffer[0], 700);
		ASSERT_EQ(buffer[1], 600);
		ASSERT_EQ(buffer[2], 500);
		ASSERT_EQ(buffer[3], 400);
	}


	TEST(MainFunctionsTests, PopFront) {
		Buffer<int> buffer(4);
		buffer.PushBack(1);
		buffer.PushBack(2);
		buffer.PushBack(3);
		buffer.PushBack(4);
	
		buffer.PopFront();
		ASSERT_EQ(*(buffer.begin()), 2);

		buffer.PopFront();
		ASSERT_EQ(*(buffer.begin()), 3);

		buffer.PopFront();
		ASSERT_EQ(*(buffer.begin()), 4);
		buffer.PopFront();
		ASSERT_THROW(buffer.PopFront(), std::out_of_range);
	}

	TEST(RelatedFunctionsTests, Swap) {
		Buffer<std::string> buffer_1 = { "first","buffer1" };
		Buffer<std::string> buffer_2 = { "second","buffer2" };
		buffer_1.Swap(buffer_2);
		ASSERT_EQ(buffer_1[0], "second");
		ASSERT_EQ(buffer_1[1], "buffer2");
		ASSERT_EQ(buffer_2[0], "first");
		ASSERT_EQ(buffer_2[1], "buffer1");
		Swap(buffer_1, buffer_2);
		ASSERT_EQ(buffer_1[0], "first");
		ASSERT_EQ(buffer_1[1], "buffer1");
		ASSERT_EQ(buffer_2[0], "second");
		ASSERT_EQ(buffer_2[1], "buffer2");
	}

	TEST(RelatedFunctionsTests, Clear) {
	 Buffer<std::string> buffer_1 = { "not","cleaned" };
	buffer_1.Clear();
	buffer_1.PushBack("already");
	buffer_1.PushBack("cleaned");
	ASSERT_EQ(buffer_1[0], "already");
	ASSERT_EQ(buffer_1[1], "cleaned");
	}

	

	TEST(IteratorsFunctionsTests, EraseOneElement) {
		Buffer<int64_t> buffer_1(20);
		buffer_1.PushBack(11212321);
		buffer_1.PushBack(3213213);
		buffer_1.PushBack(23213);
		buffer_1.PushBack(-123213213);
		buffer_1.Erase(buffer_1.begin());
		buffer_1.Erase(buffer_1.end() - 1);
		ASSERT_EQ(*(buffer_1.begin()), 3213213);
		ASSERT_EQ(*(buffer_1.end() - 1), 23213);
	}


	TEST(IteratorsFunctionsTests, EraseMany) {
		Buffer<int64_t> buffer_1(20);
		buffer_1.PushBack(11212321);
		buffer_1.PushBack(3213213);
		buffer_1.PushBack(23213);
		buffer_1.PushBack(-123213213);
		buffer_1.Erase(buffer_1.begin(), buffer_1.begin()+3);
		
		ASSERT_EQ(*(buffer_1.begin()), -123213213);
		ASSERT_EQ(*(buffer_1.end() - 1), -123213213);
	}
	TEST(IteratorsFunctionsTests, IteratorAssign) {
		Buffer<int64_t> buffer_1(20);
		buffer_1.PushBack(11212321);
		buffer_1.PushBack(3213213);
		buffer_1.PushBack(23213);
		buffer_1.PushBack(-123213213);
		Buffer<int64_t> buffer_2(40);
		buffer_2.Assign(buffer_1.begin(), buffer_1.end());
		ASSERT_EQ(buffer_1, buffer_2);
	}

	TEST(RelatedFunctionsTests, InitializerListAssign) {
		Buffer<int64_t> buffer_1(20);
		buffer_1.Assign({ 0,1,2,3,4,5 });
		for (int i=0;i<6;i++) {
			ASSERT_EQ(buffer_1[i], i);
		}
	}
	TEST(RelatedFunctionsTests, AssignWithValue) {
		Buffer<int64_t> buffer_1(40);
		buffer_1.Assign(buffer_1.Size(),555);
		for (int i = 0; i < buffer_1.Size(); i++) {
			ASSERT_EQ(buffer_1[i],555);
		}
	}
	TEST(RelatedFunctionsTests, ManyNotImportantFunctions) {
		Buffer<std::string> buf(10);
		ASSERT_THROW(buf.Front(), std::out_of_range);
		buf.PushBack("hello");
		ASSERT_EQ(buf.Front(), "hello");
		buf.PopBack();
		ASSERT_THROW(buf.Back(), std::out_of_range);
		buf.PushBack("there");
		ASSERT_EQ(buf.Back(), "there");

}

	TEST(IteratorTests, SimpleIteration) {
		int value = 1;
		Buffer buf = { 1,2,3,4,5 };
		for (auto i=buf.begin();i!=buf.end();++i) {
			ASSERT_EQ(*i, value);
		++value;
	}
}

	TEST(IteratorTests, ReverseIterators) {
		int value = 5;
		Buffer buf = { 1,2,3,4,5 };
		for (auto i = buf.rbegin(); i != buf.rend(); ++i) {
			ASSERT_EQ(*i, value);
			--value;
		}
	}
	TEST(EqualityTest,	EqualsOrNot) {
		Buffer<std::string> first = { "first","second" };
		Buffer<std::string> second = { "first","second" };
		ASSERT_EQ(first, second);
		second.PushBack("hello");
		ASSERT_NE(first, second);
}

	TEST(SwapTest, SwapBuffers) {
		Buffer<std::string> first = { "from","first" };
		Buffer<std::string> second = { "from","second" };
		first.Swap(second);
		ASSERT_EQ(first[1], "second");
		ASSERT_EQ(second[1], "first");
		Swap(first, second);
		ASSERT_EQ(first[1], "first");
		ASSERT_EQ(second[1], "second");
}