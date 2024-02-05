#include "ForeCaster.h"

std::string GetWeather(int weather_code) {
	switch (weather_code) {
	case 0:
		return "Clear sky";

	case 1:
	case 2:
	case 3:
		return "Mainly clear";

	case 45:
	case 48:
		return "Foggy";

	case 51:
	case 53:
	case 55:
	case 56:
	case 57:
		return "Drizzle";

	case 61:
	case 63:
	case 65:
	case 66:
	case 67:
		return "Raining";

	case 71:
	case 73:
	case 75:
	case 77:
		return "Snow";

	case 80:
	case 81:
	case 82:
	case 85:
	case 86:
		return "Snow or rain showers";

	case 95:
	case 96:
	case 99:
		return "Mainly clear, partly clouds expected";

	default:
		return "No weather available";
	}
}

void ForeCaster::DecodeWeatherCode(const json& current_day, std::string& cur_weather, const size_t idx) noexcept {
	decltype(auto) weather_code = current_day["hourly"]["weathercode"][idx].get<int>();

	cur_weather = GetWeather(weather_code);
}

void ForeCaster::CreatePieceOfTable(const size_t column, const size_t moment_of_time, ConsoleTable& table,
	const json& weather_json, const std::string& weather, const size_t current_day) const noexcept {
	if (weather == "Foggy") {
		table[Rows::kFirstRow][column] = weather + "\n" + R"(   .--.
  .-(    ).
  (___.__)__) )";
	}

	else  if (weather == "Clear sky" or weather == "Mainly clear" or weather == "Mainly clear,partly clouds expected") {
		table[Rows::kFirstRow][column] = weather + "\n" +
			R"(\   /
       .-.       
     --(   )--     
      '-'      
/   \

)";
	}

	else  if (weather == "Drizzle" or weather == "Raining") {
		table[Rows::kFirstRow][column] = weather + "\n" + R"(   .--.
  .-(    ).
  (___.__)__)
  '  '  '  '
 '  '  '  ' )";
	}

	else  if (weather == "Snow" or weather == "Snow or rain showers") {
		table[Rows::kFirstRow][column] = weather + "\n" + R"(   .--.
  .-(    ).
  (___.__)__)
  *  *  *  *
 *  *  *  * )";
	}

	decltype(auto) temperature = weather_json["hourly"]["temperature_2m"][moment_of_time + (kHoursPerDay *
		current_day)];
	decltype(auto) apparent_temperature = weather_json["hourly"]["apparent_temperature"][moment_of_time + (kHoursPerDay
		*
		current_day)];
	decltype(auto) wind_speed = weather_json["hourly"]["windspeed_10m"][moment_of_time + (kHoursPerDay * current_day)];
	decltype(auto) visibility = weather_json["hourly"]["visibility"][moment_of_time + (kHoursPerDay * current_day)];
	decltype(auto) precipitation = weather_json["hourly"]["precipitation"][moment_of_time + (kHoursPerDay *
		current_day)];
	decltype(auto) precipitation_probability = weather_json["hourly"]["precipitation_probability"][moment_of_time +
		kHoursPerDay
		* current_day];


	const bool temperature_is_valid = !temperature.is_null();
	const bool apparent_temperature_is_valid = !apparent_temperature.is_null();
	const bool wind_speed_is_valid = !wind_speed.is_null();
	const bool visibility_is_valid = !visibility.is_null();
	const bool precipitation_is_valid = !precipitation.is_null();
	const bool precipitation_probability_is_valid = !precipitation_probability.is_null();

	if (temperature_is_valid and apparent_temperature_is_valid) {
		table[kSecondRow][column] = "Temperature is " + std::to_string(temperature.get<int>()) + "(" + std::to_string(
			apparent_temperature.get<int>()) + ")" + " C\370";
	}


	if (wind_speed_is_valid) {
		table[kThirdRow][column] = "Windspeed is " + std::to_string(wind_speed.get<int>()) + " km\\h";
	}
	if (visibility_is_valid) {
		table[kFourthRow][column] = "Visibility " + std::to_string(
			visibility.get<int>() / kMetersInKilometer) + " km";
	}

	if (precipitation_is_valid and precipitation_probability_is_valid) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << precipitation.get<double>();
		table[kFifthRow][column] = "Precipitation is "
			+ oss.str() + "mm" + " | "
			+ std::to_string(precipitation_probability.get<int>()) +
			"%";
	}
}

void ForeCaster::CreateDayTable(ConsoleTable& table, const json& weather_json,
                                std::vector<std::pair<
	                                std::string, ConsoleTable>>& weather,
                                const size_t current_day) const noexcept {
	const std::string input_time_string = weather_json["hourly"]["time"][
		kHoursPerDay * current_day];

	std::tm time{};

	std::stringstream ss(input_time_string);
	ss >> std::get_time(&time, "%Y-%m-%dT%H:%M");

	std::stringstream ss2;
	ss2 << std::put_time(&time, "%d %B,%Y");

	table[kZeroRow][kZeroColumn] = "Morning";
	CreatePieceOfTable(kZeroColumn, kMorning, table, weather_json,
	                   morning_weather_, current_day);


	table[kZeroRow][kFirstColumn] = "Day";
	CreatePieceOfTable(kFirstColumn, kDay, table, weather_json, day_weather_,
	                   current_day);


	table[kZeroRow][kSecondColumn] = "Afternoon";
	CreatePieceOfTable(kSecondColumn, kNoon, table, weather_json, noon_weather_,
	                   current_day);


	table[kZeroRow][kThirdColumn] = "Night";
	CreatePieceOfTable(kThirdColumn, kNight, table, weather_json, night_weather_,
	                   current_day);
	weather.emplace_back(ss2.str(), table);
}

[[nodiscard]] cpr::Response ForeCaster::GetAnswerFromNinjaApi(
	const std::string& city_name) const noexcept {
	auto response = Get(cpr::Url{"https://api.api-ninjas.com/v1/city"},
	                    cpr::Header{
		                    {
			                    "X-API-Key",
			                    "fdDDs60tuLqhf909ucROlA==XPNDvUhSgCkDyMx3"
		                    }
	                    },
	                    cpr::Parameters{{"name", city_name}});
	return response;
}

cpr::Response ForeCaster::GetAnswerFromWeatherApi() noexcept {
	auto response = Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
	                    cpr::Parameters{
		                    {
			                    "longitude",
			                    std::to_string(
				                    info_to_weather_api_[0]["longitude"].get<
					                    double>())
		                    },

		                    {
			                    "latitude",
			                    std::to_string(
				                    info_to_weather_api_[0]["latitude"].get<
					                    double>())
		                    },

		                    {
			                    "hourly",
			                    "temperature_2m,relativehumidity_2m,apparent_temperature,precipitation_probability,precipitation,weathercode,visibility,windspeed_10m"
		                    },
		                    {
			                    "forecast_days",
			                    std::to_string(amount_of_forecast_days_)
		                    },


	                    }
	);
	return response;
}


[[nodiscard]] cpr::Response ForeCaster::SendRequestToNinjaApi(
	const std::string& city_name) const noexcept {
	auto response = GetAnswerFromNinjaApi(city_name);

	while (response.status_code != kCorrectAPIAnswer) {
		std::cout <<
			"Ninja API didn't answered correctly.Probably your API key is invalidated or you typed incorrect name of city. New request to API will be conducted in 30 seconds,"
			"	  but you should check for key and city and restart.probably\n";
		std::this_thread::sleep_until(
			std::chrono::system_clock::now() + std::chrono::seconds(30));
		response = GetAnswerFromNinjaApi(city_name);
	}
	return response;
}

[[nodiscard]] cpr::Response ForeCaster::SendRequestToWeatherApi(
	const cpr::Response& response_from_ninja_api) noexcept {
	info_to_weather_api_ = json::parse(response_from_ninja_api.text);

	cpr::Response current_weather = GetAnswerFromWeatherApi();
	while (current_weather.status_code != kCorrectAPIAnswer) {
		std::cout <<
			"Weather API didn't answered correctly. New request to API will be conducted in 30 seconds\n";
		std::this_thread::sleep_until(
			std::chrono::system_clock::now() + std::chrono::seconds(
				kSleepFor30Sec));
		current_weather = GetAnswerFromWeatherApi();
	}
	return current_weather;
}

std::vector<std::pair<std::string, ConsoleTable>> ForeCaster::ParseWeather(
	const cpr::Response& current_weather) noexcept {
	const json basic_weather_json = json::parse(current_weather.text);
	std::vector<std::pair<std::string, ConsoleTable>> weather;
	for (size_t i = 0; i < amount_of_forecast_days_; ++i) {
		ConsoleTable table(0, 0, samilton::Alignment::centre);

		DecodeWeatherCode(basic_weather_json, morning_weather_,
		                  kMorning + kHoursPerDay * i);
		DecodeWeatherCode(basic_weather_json, day_weather_,
		                  kDay + kHoursPerDay * i);
		DecodeWeatherCode(basic_weather_json, noon_weather_,
		                  kNoon + kHoursPerDay * i);
		DecodeWeatherCode(basic_weather_json, night_weather_,
		                  kNight + kHoursPerDay * i);

		CreateDayTable(table, basic_weather_json, weather, i);
	}
	return weather;
}

std::vector<std::pair<std::string, ConsoleTable>> ForeCaster::SendRequestToApi(
	const std::string& city_name) noexcept {
	decltype(auto) response_from_ninja_api = SendRequestToNinjaApi(city_name);

	decltype(auto) current_weather = SendRequestToWeatherApi(
		response_from_ninja_api);


	return ParseWeather(current_weather);
}

void ForeCaster::PrintNextDay() noexcept {
	system("cls");

	std::cout << "Weather in " << all_cities_[current_city_] << " is\n ";

	for (size_t i = 0; i < how_much_to_show_per_city_[current_city_]; ++i)
		std::cout << "\n" << weather_for_all_days_[current_city_][i].first << "\n"
			<< weather_for_all_days_[current_city_][
				i].second;
}

void ForeCaster::ParceUserInput() noexcept {
	const int button_pressed = _getch();

	switch (button_pressed) {
	case '+':
		if (how_much_to_show_per_city_[current_city_] < amount_of_forecast_days_) {
			++how_much_to_show_per_city_[current_city_];
			PrintNextDay();
		}
		else {
			std::cout << "Too many days displayed already. I can provide only "
				<< amount_of_forecast_days_ <<
				" days forecast\n";
		}
		break;

	case '-':
		if (how_much_to_show_per_city_[current_city_] != kNothingToShow) {
			--how_much_to_show_per_city_[current_city_];
			PrintNextDay();
		}
		break;

	case 'n':
		if (current_city_ + 1 < weather_for_all_days_.size()) {
			++current_city_;
			if (how_much_to_show_per_city_[current_city_] == kNothingToShow)
				++how_much_to_show_per_city_[current_city_];
			PrintNextDay();
		}
		else
			std::cout << "All the cities have already been shown\n";
		break;

	case 'p':
		if (current_city_ - 1 < weather_for_all_days_.size()) {
			--current_city_;

			PrintNextDay();
		}
		else
			std::cout << "It's already first city\n";
		break;

	case kESCButton:
		std::quick_exit(EXIT_SUCCESS);

	default:
		std::cout << " Unsupported button pressed\n";
	}
}

void ForeCaster::UpdateInfo() noexcept {
	how_much_to_show_per_city_.assign(all_cities_.size(), kNothingToShow);

	weather_for_all_days_.reserve(all_cities_.size());

	for (const auto& i : all_cities_)
		weather_for_all_days_.emplace_back(SendRequestToApi(i));

	++how_much_to_show_per_city_[kNothingToShow];
	PrintNextDay();
}

void ForeCaster::ForeCast() noexcept {
	

	if (!exists(path_to_json_)) {
		std::cerr << "Error: file not found" << std::endl;
		return;
	}

	std::ifstream file(path_to_json_);


	parsed_input_ = json::parse(file);

	amount_of_forecast_days_ = parsed_input_["amount_of_forecast_days"].get<
		size_t>();

	for (const auto& i : parsed_input_["cities"])
		all_cities_.emplace_back(i);

	const auto start = std::chrono::system_clock::now();

	UpdateInfo();


	while (true) {
		auto end = std::chrono::system_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(end - start).
			count() >= parsed_input_["update_time"].get<
			size_t>())
			UpdateInfo();
		ParceUserInput();
	}
}

ForeCaster::ForeCaster(std::filesystem::path path) noexcept : path_to_json_(std::move(path)),
amount_of_forecast_days_(
	kNothingToShow) {}
