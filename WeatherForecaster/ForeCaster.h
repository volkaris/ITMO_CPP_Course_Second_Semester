#pragma once

#include "CppConsoleTable.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <conio.h>

using ConsoleTable = samilton::ConsoleTable;
using json = nlohmann::json;

enum  Columns {
	kZeroColumn = 0,
	kFirstColumn = 1,
	kSecondColumn = 2,
	kThirdColumn = 3
};

enum  Rows {
	kZeroRow = 0,
	kFirstRow = 1,
	kSecondRow = 2,
	kThirdRow = 3,
	kFourthRow = 4,
	kFifthRow = 5
};

enum  Utility {
	kHoursPerDay = 24,
	kMetersInKilometer = 1000,
	kCorrectAPIAnswer = 200,
	kSleepFor30Sec = 30,
	kNothingToShow = 0,
	kESCButton = 27
};

enum  TimeOfDay {
	kMorning = 5,
	kDay = 12,
	kNoon = 16,
	kNight = 22
};

class ForeCaster {

	size_t current_city_ = 0;
	std::vector<std::string> all_cities_;
	std::vector<std::vector<std::pair<std::string, ConsoleTable>>> weather_for_all_days_;
	std::vector<size_t> how_much_to_show_per_city_;
	std::filesystem::path path_to_json_;
	json parsed_input_;
	size_t amount_of_forecast_days_;
	std::string morning_weather_;
	std::string day_weather_;
	std::string noon_weather_;
	std::string night_weather_;
	json info_to_weather_api_;

	void DecodeWeatherCode(const json& current_day, std::string& cur_weather, const size_t idx) noexcept;

	void CreatePieceOfTable(const size_t column, const size_t moment_of_time, ConsoleTable& table,
		const json& weather_json, const std::string& weather, const size_t current_day) const noexcept;


	void CreateDayTable(ConsoleTable& table, const json& weather_json,
		std::vector<std::pair<std::string, ConsoleTable>>& weather, const size_t current_day) const noexcept;

	[[nodiscard]] cpr::Response GetAnswerFromNinjaApi(const std::string& city_name) const noexcept;

	[[nodiscard]] cpr::Response GetAnswerFromWeatherApi()  noexcept;

	[[nodiscard]] cpr::Response SendRequestToNinjaApi(const std::string& url) const noexcept;


	[[nodiscard]] cpr::Response SendRequestToWeatherApi(const cpr::Response& response_from_ninja_api)  noexcept;


	std::vector<std::pair<std::string, ConsoleTable>> ParseWeather(const cpr::Response& current_weather) noexcept;

	std::vector<std::pair<std::string, ConsoleTable>> SendRequestToApi(const std::string& city_name) noexcept;

	void PrintNextDay() noexcept;

	void ParceUserInput() noexcept;

	void UpdateInfo() noexcept;

public:
	void ForeCast() noexcept;

	explicit ForeCaster(std::filesystem::path path) noexcept;
};
