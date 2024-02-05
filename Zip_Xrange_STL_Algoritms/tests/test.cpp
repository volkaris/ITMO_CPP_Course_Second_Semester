#include "lib/xrange.h"
#include "lib/stl_algoritms.h"
#include "lib/zip.h"

#include <gtest/gtest.h>

constexpr auto is_even = [](int i) {
	return i % 2 == 0;
};
constexpr auto is_positive = [](int i) {
	return i > 0;
};
constexpr auto strong_ascending_order = [](int x, int y) {
	return x < y;
};
constexpr auto strong_decending_order = [](int x, int y) {
	return x > y;
};
constexpr auto not_strong_ascending_order = [](int x, int y) {
	return x <= y;
};

class MyInt {
public:
	MyInt(int i) : i_(i) {}

	MyInt& operator++() {
		++i_;
		return *this;
	}

	MyInt operator++(int) {
		MyInt tmp = *this;
		++(*this);
		return tmp;
	}

	[[nodiscard]] int GetValue() const {
		return i_;
	}


	auto operator<=>(const MyInt&) const = default;

	void operator+=(const MyInt& value) {
		i_ += value.i_;
	}

private:
	int i_;
};


TEST(ZipperTests, EqualLenghtTest) {
	std::list l = {1, 2, 3, 4};
	std::vector v = {'a', 'b', 'c', 'd'};

	auto zipped = zip(l, v);
	ASSERT_EQ(*zipped.begin(), std::make_pair(1, 'a'));
	ASSERT_EQ(*(zipped.begin()+1), std::make_pair(2, 'b'));
	ASSERT_EQ(*(zipped.begin() + 2), std::make_pair(3, 'c'));
	ASSERT_EQ(*(zipped.begin() + 3), std::make_pair(4, 'd'));
}


TEST(ZipperTests, FirstHaveBiggerLenghtTest) {
	std::set l = {1, 2, 3, 4, 5, 6, 7, 8, 10, 11};
	std::vector v = {"first", "second"};

	auto zipped = zip(l, v);

	ASSERT_EQ((*zipped.begin()).first, 1);
	ASSERT_EQ((*zipped.begin()).second, std::string("first"));

	ASSERT_EQ((*(zipped.begin()+1)).first, 2);
	ASSERT_EQ((*(zipped.begin() + 1)).second, std::string("second"));
	EXPECT_DEATH(*(zipped.begin() + 2), ".*");
}

TEST(ZipperTests, SecondHaveBiggerLenghtTest) {
	std::multiset l = {1, 1, 4, 4};
	std::vector v = {144.5, 155.4, 12323.445, 1234.12, 100500.54, 321312.23123};

	auto zipped = zip(l, v);
	ASSERT_EQ(*zipped.begin(), std::make_pair(1, 144.5));
	ASSERT_EQ(*(zipped.begin() + 1), std::make_pair(1, 155.4));
	ASSERT_EQ(*(zipped.begin() + 2), std::make_pair(4, 12323.445));
	ASSERT_EQ(*(zipped.begin() + 3), std::make_pair(4, 1234.12));
	EXPECT_DEATH(*(zipped.begin() + 4), ".*");
}

TEST(XRangeTests, StartEndTest) {
	auto x = xrange(1.5, 5.5);
	std::vector<double> v{x.begin(), x.end()};

	for (double i = 1.5; i < 5.5; ++i) {
		ASSERT_EQ(v[ static_cast<int> (floor (i))-1], i);
	}
}

TEST(XRangeTests, WithoutStartTest) {
	auto x = xrange(4);
	std::vector<int> v{x.begin(), x.end()};

	for (int i = 1; i < 5; ++i) {
		ASSERT_EQ(v[i-1], i-1);
	}
}

TEST(XRangeTests, StartEndStepTest) {
	auto i = xrange(1, 6, 2);

	for (int j = 0; j < 3; ++j) {
		ASSERT_EQ(*(i.begin() + j), 1+2*j);
	}
}

TEST(XRangeTests, CustomClassTest) {
	MyInt i(1);
	MyInt i_(5);
	auto x = xrange(i, i_, 1);

	for (int j = 1; j < 5; ++j) {
		ASSERT_EQ((*(x.begin() + (j-1))).GetValue(), j);
	}
}

TEST(XRangeTests, NegativeStepTest) {
	auto i = xrange(5, 1, -1);

	for (int j = 5; j > 1; --j) {
		ASSERT_EQ(*(i.begin() + (5-j)), j);
	}

	ASSERT_ANY_THROW(*(i.begin() + 4));
}

TEST(XRangeTests, NegativeStartTest) {
	auto i = xrange(-5, 1, 1);

	int k = 0;

	for (int j = -5; j < 1; ++j) {
		ASSERT_EQ(*(i.begin() + k), j);
		++k;
	}

	ASSERT_ANY_THROW(*(i.begin() + 6));
}

TEST(XRangeTests, NegativeStartEndTest) {
	auto i = xrange(-5, -1, 1);

	int k = 0;

	for (int j = -5; j < -1; ++j) {
		ASSERT_EQ(*(i.begin() + k), j);
		++k;
	}
}


TEST(STLAlgoritmTests, AllOf) {
	std::vector vec{1, 2, 3, 4, 5};
	std::vector vec2{2, 4, 6, 8, 10};
	ASSERT_FALSE(
		stl_algorithms::all_of(vec.cbegin(), vec.cend(), is_even));
	ASSERT_TRUE(
		stl_algorithms::all_of(vec2.cbegin(), vec2.cend(), is_even));
}

TEST(STLAlgoritmTests, AnyOf) {
	std::vector vec{1, 2, 3, 4, 5};
	std::vector vec2{1, 3, 5, 7, 9};
	ASSERT_TRUE(
		stl_algorithms::any_of(vec.cbegin(), vec.cend(), is_even));
	ASSERT_FALSE(
		stl_algorithms::any_of(vec2.cbegin(), vec2.cend(), is_even));
}


TEST(STLAlgoritmTests, OneOf) {
	std::vector vec{1, 2, 3, 4, 5};
	std::vector vec2{1, 2, 3, 7, 9};
	std::vector vec3{1, 19, 3, 7, 9};
	ASSERT_FALSE(
		stl_algorithms::one_of(vec.cbegin(), vec.cend(), is_even));
	ASSERT_FALSE(
		stl_algorithms::one_of(vec3.cbegin(), vec3.cend(), is_even));
	ASSERT_TRUE(
		stl_algorithms::one_of(vec2.cbegin(), vec2.cend(), is_even));
}

TEST(STLAlgoritmTests, NoneOf) {
	std::vector vec{1, 2, 3, 4, 5};
	std::vector vec2{1, 2, 3, 7, 9};
	std::vector vec3{1, 19, 3, 7, 9};
	ASSERT_FALSE(
		stl_algorithms::none_of(vec.cbegin(), vec.cend(), is_even));
	ASSERT_TRUE(
		stl_algorithms::none_of(vec3.cbegin(), vec3.cend(), is_even));
	ASSERT_FALSE(
		stl_algorithms::none_of(vec2.cbegin(), vec2.cend(), is_even));
}

TEST(STLAlgoritmTests, FindNot) {
	std::vector vec1{10};
	std::vector vec2{1, 3, 5, 7, 9};
	ASSERT_EQ(stl_algorithms::find_not(vec2.cbegin(), vec2.cend(), 2),
	          vec2.cbegin());
	ASSERT_EQ(stl_algorithms::find_not(vec1.cbegin(), vec1.cend(), 10),
	          vec1.cend());
}

TEST(STLAlgoritmTests, FindBackwards) {
	std::vector vec1{10, 9, 8, 7};
	std::vector vec2{1, 3, 5, 7, 9};
	ASSERT_EQ(stl_algorithms::find_backward(vec1.cbegin(), vec1.cend(), 2),
	          vec1.cend());
	ASSERT_EQ(stl_algorithms::find_backward(vec2.cbegin(), vec2.cend(), 7),
	          vec2.cend()-2);
}

TEST(STLAlgoritmTests, IsSorted) {
	int a[] = {1, 2, 3, 4, 5};
	int b[] = {5, 4, 3, 2, 1};
	int d[] = {1, 1, 2, 2, 3, 3};
	std::vector vec = {1};
	ASSERT_TRUE(
		stl_algorithms::is_sorted(std::begin(a), std::end(a),
			strong_ascending_order));
	ASSERT_TRUE(
		stl_algorithms::is_sorted(std::begin(b), std::end(b),
			strong_decending_order));
	ASSERT_FALSE(stl_algorithms::is_sorted(std::begin(b), std::end(b)));
	ASSERT_TRUE(
		stl_algorithms::is_sorted(std::begin(d), std::end(d),
			not_strong_ascending_order));
	ASSERT_TRUE(stl_algorithms::is_sorted(vec.begin(), vec.end()));
}

TEST(STLAlgoritmTests, IsPartitioned) {
	std::vector<int> v1{2, 4, 6, -1, -3, -5};
	ASSERT_TRUE(
		stl_algorithms::is_partitioned(v1.begin(), v1.end(), is_even));
	ASSERT_TRUE(
		stl_algorithms::is_partitioned(v1.begin(), v1.end(), is_positive));

	std::vector<int> v2{1, 2, 3, 4, 5, 6};
	ASSERT_TRUE(
		!stl_algorithms::is_partitioned(v2.begin(), v2.end(), is_even));
	ASSERT_TRUE(
		stl_algorithms::is_partitioned(v2.begin(), v2.end(), is_positive));
}

TEST(STLAlgoritmTests, IsPalindrome) {
	std::vector<std::string> vec2 = {"hello", "w", "world", "hell"};

	ASSERT_FALSE(stl_algorithms::is_palindrome(std::begin(vec2), std::end(vec2),
		[](const std::string& x) {
		return x.length() > 3;
		}));

	std::vector<int> v = {1, 2, 3, 4, 5};
	ASSERT_TRUE(
		stl_algorithms::is_palindrome(std::begin(v), std::end(v), is_even));

	std::vector<int> v3 = {1, 2, 3, 19, 6};
	ASSERT_FALSE(
		stl_algorithms::is_palindrome(std::begin(v3), std::end(v3), is_even));
}
