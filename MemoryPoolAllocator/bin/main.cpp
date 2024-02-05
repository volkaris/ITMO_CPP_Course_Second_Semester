

#include <iostream>

#include "lib/Tests.h"



#include <chrono>
#include <fstream>
#include <list>
#include <ostream>
#include <lib/PoolsAllocator.h>

int main() {
	std::ofstream file("results.txt");

	std::vector<std::string, ManyPoolsAllocator<std::string>> vector_with_many_pools(ManyPoolsAllocator<std::string>(3, 6000000, 1500000, 700000, 1400000, 9000000, 1407000));
	std::vector<std::string> std_vector;

	std::list< std::string, ManyPoolsAllocator<std::string>> list_with_many_pools(ManyPoolsAllocator<std::string>(3, 6000000, 50, 700000, 75, 9000000, 55));
	std::list<std::string> std_list;

	auto first_test_with_vector_many_pools = FirstCalculation(vector_with_many_pools);
	auto first_test_with_vector_std_alloc = FirstCalculation(std_vector);

	auto first_test_with_list_many_pools = FirstCalculation(list_with_many_pools);
	auto first_test_with_list_std_alloc = FirstCalculation(std_list);

	auto second_test_with_list_many_pools = SecondCalculation(list_with_many_pools);
	auto second_test_with_list_std_alloc = SecondCalculation(std_list);

	auto third_test_with_list_many_pools = ThirdCalculation(list_with_many_pools);
	auto third_test_with_list_std_alloc = ThirdCalculation(std_list);

	file << "Results of  test of VECTOR with many pools allocator is\n";
	file << "Average amount is " << first_test_with_vector_many_pools.first << "\n";
	file << "Mediana amount is " << first_test_with_vector_many_pools.second << "\n\n";

	file << "Results of  test of std allocator is\n";
	file << "Average amount is " << first_test_with_vector_std_alloc.first << "\n";
	file << "Mediana amount is " << first_test_with_vector_std_alloc.second << "\n\n";

	file << "Results of first test of LIST with many pools allocator is\n";
	file << "Average amount is " << first_test_with_list_many_pools.first << "\n";
	file << "Mediana amount is " << first_test_with_list_many_pools.second << "\n\n";

	file << "Results of first test of std allocator is\n";
	file << "Average amount is " << first_test_with_list_std_alloc.first << "\n";
	file << "Mediana amount is " << first_test_with_list_std_alloc.second << "\n\n";

	file << "Results of second test of list with many pools allocator is\n";
	file << "Average amount is " << second_test_with_list_many_pools.first << "\n";
	file << "Mediana amount is " << second_test_with_list_many_pools.second << "\n\n";

	file << "Results of second test of std allocator is\n";
	file << "Average amount is " << second_test_with_list_std_alloc.first << "\n";
	file << "Mediana amount is " << second_test_with_list_std_alloc.second << "\n\n";

	file << "Results of third test of many pools allocator is\n";
	file << "Average amount is " << third_test_with_list_many_pools.first << "\n";
	file << "Mediana amount is " << third_test_with_list_many_pools.second << "\n\n";

	file << "Results of third test of std allocator is\n";
	file << "Average amount is " << third_test_with_list_std_alloc.first << "\n";
	file << "Mediana amount is " << third_test_with_list_std_alloc.second << "\n\n";
}
