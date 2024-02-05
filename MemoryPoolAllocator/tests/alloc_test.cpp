
#include <gtest/gtest.h>
#include <tuple>

#include "lib/PoolsAllocator.h"

TEST(ValidData, Vector) {
	 std::vector<std::string, ManyPoolsAllocator<std::string>> vector_with_many_pools(ManyPoolsAllocator<std::string>(3, 600000, 150000, 700000, 140000, 9000000, 100500));
	 vector_with_many_pools.resize(200);
	for (size_t i=0;i<vector_with_many_pools.size();++i) {
		vector_with_many_pools[i] = "hello"+std::to_string(i);
	}
	for (size_t i = 0; i < vector_with_many_pools.size(); ++i) {
	ASSERT_EQ(vector_with_many_pools[i], "hello" + std::to_string(i));
	}
}

TEST(ValidData, List) {
	std::list< int, ManyPoolsAllocator<int>> list_with_many_pools(ManyPoolsAllocator<int>(3, 6000000, 50, 700000, 75, 9000000, 55));
	for (size_t i = 0; i < list_with_many_pools.size(); ++i) {
		list_with_many_pools.push_back(i);
	}
	size_t idx = 0;
	for (const auto& i:list_with_many_pools){
		ASSERT_EQ(i, idx);
		++idx;
	}
}