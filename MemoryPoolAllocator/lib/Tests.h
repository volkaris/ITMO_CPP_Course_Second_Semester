#pragma once

#include "Pool.h"

#include <chrono>
#include <numeric>

template <typename Container>
auto FirstCalculation(Container container);

template <typename Container>
auto SecondCalculation(Container container);

template <typename Container>
auto ThirdCalculation(Container container);



template <typename Container>
auto FirstCalculation(Container container) {
	std::vector<size_t> average_and_mediana;

	for (size_t i = 0; i < 1000; ++i) {
		auto start = std::chrono::system_clock::now();

		container.resize(30000);

		for (auto& j : container) {
			j = "hello world";
		}


		for (auto j = 0; j < 30000; ++j)
			container.pop_back();

		auto end = std::chrono::system_clock::now();

		average_and_mediana.push_back(
			std::chrono::duration_cast<std::chrono::microseconds>(end - start).
			count());
	}
	std::ranges::sort(average_and_mediana);


	const size_t sum = std::accumulate(average_and_mediana.begin(),
		average_and_mediana.end(),
		static_cast<size_t>(0)
	);
	return std::make_pair(sum / average_and_mediana.size(),
		average_and_mediana[average_and_mediana.size() / 2]);

}

template <typename Container>
auto SecondCalculation(Container container) {
	std::vector<size_t> average_and_mediana;

	for (size_t i = 0; i < 1000; ++i) {
		auto start = std::chrono::system_clock::now();

		container.assign(85000, "test");
		container.erase(container.begin(), container.end());

		auto end = std::chrono::system_clock::now();

		average_and_mediana.push_back(
			std::chrono::duration_cast<std::chrono::microseconds>(end - start).
			count());
	}
	std::ranges::sort(average_and_mediana);

	const size_t sum = std::accumulate(average_and_mediana.begin(),
		average_and_mediana.end(),
		static_cast<size_t>(0));
	return std::make_pair(sum / average_and_mediana.size(),
		average_and_mediana[average_and_mediana.size() / 2]);
}

template <typename Container>
auto ThirdCalculation(Container container) {
	std::vector<size_t> average_and_mediana;

	for (size_t i = 0; i < 1000; ++i) {
		auto start = std::chrono::system_clock::now();

		for (auto j = 0; j < 50000; ++j)
			container.emplace_front("hello world");


		for (auto j = 0; j < 50000; ++j)
			container.pop_front();

		auto end = std::chrono::system_clock::now();

		average_and_mediana.push_back(
			std::chrono::duration_cast<std::chrono::microseconds>(end - start).
			count());
	}
	std::ranges::sort(average_and_mediana);

	size_t sum = std::accumulate(average_and_mediana.begin(),
		average_and_mediana.end(),
		static_cast<size_t>(0));

	return std::make_pair(sum / average_and_mediana.size(),
		average_and_mediana[average_and_mediana.size() / 2]);

}

