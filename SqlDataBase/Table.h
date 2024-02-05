#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

class Table {
public:
	Table() = default;

	template <typename... Args>
	explicit Table(const std::string& table_name, const std::string& primary_key,
	               Args&&... columns) {
		name_of_table_ = table_name;


		(names_of_columns_.emplace_back(std::forward<Args>(columns)), ...);

		amount_of_columns_ = sizeof...(Args);

		CalculateIndexOfPrimaryKey(primary_key);
	}


	template <typename... Args>
	explicit Table(const std::string& table_name, const std::string& primary_key, Table* foreign_key,
	               Args&&... columns) : Table(table_name, primary_key, columns) {
		foreign_key_ = foreign_key;
	}


	explicit Table(const std::string& table_name, const std::string& primary_key,
	               std::vector<std::string>& columns) {
		name_of_table_ = table_name;


		names_of_columns_ = std::move(columns);


		amount_of_columns_ = names_of_columns_.size();

		CalculateIndexOfPrimaryKey(primary_key);
	}


	void AddRow(std::vector<std::string>& row) {
		if (row.size() < names_of_columns_.size()) {
			row.resize(names_of_columns_.size(), "NULL");
		}

		table_.emplace_back(std::move(row));

		++amount_of_rows_;


		size_t offset = index_of_primary_key_;


		decltype(auto) primary_key = table_[table_.size() - 1][offset];


		if (primary_keys_.contains(primary_key))
			throw std::invalid_argument(
				"Value with this key already presented in table");

		if (primary_key != "NULL") {
			primary_keys_.insert(primary_key);
		}
	}

	template <typename... Args>
	void AddRow(Args&&... args) {
		std::vector<std::string> result;

		(result.emplace_back(ToString(std::forward<Args>(args))), ...);

		AddRow(result);
	}


	[[nodiscard]] size_t GetAmountOfColumns() const noexcept {
		return amount_of_columns_;
	}


	[[nodiscard]] size_t GetAmountOfRows() const noexcept {
		return amount_of_rows_;
	}


	[[nodiscard]] const std::vector<std::string>& GetRow(size_t idx) const {
		return table_[idx];
	}


	friend class DataBase;
	friend std::ostream& operator <<(std::ostream& os, const DataBase& data_base);

private:
	std::string name_of_table_;
	size_t amount_of_columns_ = 0;
	size_t amount_of_rows_ = 0;
	std::unordered_set<std::string> primary_keys_;
	std::vector<std::vector<std::string>> table_;
	std::vector<std::string> names_of_columns_;
	size_t index_of_primary_key_ = 0;
	Table* foreign_key_ = nullptr;

	template <typename T>
	[[nodiscard]] std::string ToString(const T& value) const noexcept {
		std::ostringstream os;
		os << value;
		return os.str();
	}

	void CalculateIndexOfPrimaryKey(const std::string& primary_key) {
		for (const auto& key : names_of_columns_) {
			if (key == primary_key) {
				break;
			}
			++index_of_primary_key_;
		}

		if (index_of_primary_key_ == names_of_columns_.size()) {
			throw std::invalid_argument(
				"In create table function you forgot to specify name of column with primary key");
		}
	}
};
