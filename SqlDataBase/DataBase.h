#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <random>

#include "Table.h"

template <typename Lambda>
class Where {
public:
	explicit Where(Lambda l) : lambda_(l) {}
	Lambda lambda_;
};


template <typename Lambda>
class On {
public:
	explicit On(Lambda l) : lambda_(l) {}
	Lambda lambda_;
};


class DataBase {
public:
	DataBase() = default;

	//tested
	template <typename... Values>
	void Insert(const std::string& table_name, Values&&... val) {
		const size_t idx = GetIndex(table_name);

		tables[idx].AddRow(std::forward<Values>(val)...);
	}

	//tested
	void Insert(const std::string& table_name, std::vector<std::string>& val) {
		const size_t idx = GetIndex(table_name);

		tables[idx].AddRow(val);
	}

	//tested
	template <typename... Strings>
	void CreateTable(const std::string& table_name,
	                 const std::string& row_with_primary_key,
	                 Strings&&... columns) {
		tables.emplace_back(table_name, row_with_primary_key,
		                    std::forward<Strings>(columns)...);
	}

	//tested
	void CreateTable(const std::string& table_name,
	                 const std::string& row_with_primary_key, std::vector<std::string>& columns) noexcept {
		tables.emplace_back(table_name, row_with_primary_key, columns);
	}

	//tested
	template <typename T, typename Predicate>
	void Update(const std::string& table_name, const std::string& columns, T& Replacement,
	            const std::string& row_for_predicate, Where<Predicate> where) noexcept {
		for (decltype(auto) table : tables)
			if (table.name_of_table_ == table_name) {
				const size_t replacement_idx = FindColumnIndex(columns, table);
				const size_t idx_for_predicate = FindColumnIndex(row_for_predicate, table);
				for (decltype(auto) rows : table.table_) {
					if (where.lambda_((rows[idx_for_predicate]))) {
						rows[replacement_idx] = Replacement;
					}
				}
			}
	}


	//tested
	template <typename T, typename Predicate>
	void Update(const std::string& table_name, const std::string& columns, T& Replacement,
	            const std::string& table_for_predicate, On<Predicate> on) noexcept {
		DataBase::Update(table_name, columns, Replacement, table_for_predicate, Where(on.lambda_));
	}


	//tested
	template <typename T>
	void Update(const std::string& table_name, const std::string& columns, T& Replacement) noexcept {
		for (decltype(auto) table : tables)
			if (table.name_of_table_ == table_name) {
				const size_t replacement_idx = FindColumnIndex(columns, table);

				for (decltype(auto) rows : table.table_) {
					rows[replacement_idx] = Replacement;
				}
			}
	}


	//tested
	void Delete(const std::string& table_name) {
		for (decltype(auto) table : tables)
			if (table.name_of_table_ == table_name) {
				table.table_.clear();
				table.primary_keys_.clear();
				table.amount_of_rows_ = 0;
				return;
			}
		throw std::invalid_argument("No such table present");
	}

	//tested
	template <typename Predicate>
	void Delete(const std::string& table_name, const std::string& column_name, Where<Predicate> pred) {
		for (decltype(auto) table : tables) {
			if (table.name_of_table_ == table_name) {
				const size_t idx_for_predicate = FindColumnIndex(column_name, table);


				for (auto it = table.table_.begin(); it != table.table_.end();) {
					if (pred.lambda_(((*it)[idx_for_predicate]))) {
						table.primary_keys_.erase(
							table.primary_keys_.find(*(it->begin() + table.index_of_primary_key_)));
						it = table.table_.erase(it);
						--table.amount_of_rows_;
					}

					else {
						++it;
					}
				}
			}
		}
	}

	//tested
	template <typename Predicate>
	void Delete(const std::string& table_name, const std::string& column_name, On<Predicate> pred) {
		Delete(table_name, column_name, Where<Predicate>(pred.lambda_));
	}


	//tested
	void DropTable(const std::string& deleted_table) {
		for (auto i = tables.begin(); i != tables.end(); ++i)
			if ((*i).name_of_table_ == deleted_table) {
				tables.erase(i);
				return;
			}
		throw std::invalid_argument("No such table present");
	}


	template <typename lambda, typename... Strings>
	void InnerJoin(const std::string& name_of_first_table, const std::string& name_of_second_table,
	               const std::string& name_of_new_table,
	               const std::string& primary_key, On<lambda> pred,
	               const std::string& first_column,
	               const std::string& second_column, Strings&&... names_of_columns) {
		DataBase::CreateTable(name_of_new_table, primary_key, names_of_columns...);
		std::vector<std::string> columns{names_of_columns...};

		Table first_table;
		Table second_table;
		size_t number_of_first_column_to_check_pred;
		size_t number_of_second_column_to_check_pred;

		std::vector<std::vector<std::string>>::iterator start1;
		std::vector<std::vector<std::string>>::iterator start2;

		Initialize(name_of_first_table, name_of_second_table, first_column, second_column,
		           first_table, second_table,
		           number_of_first_column_to_check_pred, number_of_second_column_to_check_pred, start1, start2);


		for (auto it1 = start1; it1 != first_table.table_.end(); ++it1) {
			for (auto it2 = start2; it2 != second_table.table_.end(); ++it2) {
				if (pred.lambda_(*(it1->begin() + number_of_first_column_to_check_pred),
				                 *(it2->begin() + number_of_second_column_to_check_pred))) {
					std::vector<std::string> what_to_add_to_table;

					for (const auto& i : columns) {
						auto is_In_first = FindColumnIndex(i, first_table);
						auto is_In_second = FindColumnIndex(i, second_table);

						if (is_In_first != -1) {
							what_to_add_to_table.emplace_back(it1->begin()[is_In_first]);
						}

						else if (is_In_second != -1) {
							what_to_add_to_table.emplace_back(it2->begin()[is_In_second]);
						}
					}

					Insert(name_of_new_table, what_to_add_to_table);

					break;
				}
			}
		}
	}


	template <typename lambda, typename... Strings>
	void LeftJoin(const std::string& name_of_first_table, const std::string& name_of_second_table,
	              const std::string& name_of_new_table,
	              const std::string& primary_key, On<lambda> pred,
	              const std::string& first_column,
	              const std::string& second_column, Strings&&... names_of_columns) {
		DataBase::CreateTable(name_of_new_table, primary_key, names_of_columns...);
		std::vector<std::string> columns{names_of_columns...};

		Table first_table;
		Table second_table;
		size_t number_of_first_column_to_check_pred;
		size_t number_of_second_column_to_check_pred;

		std::vector<std::vector<std::string>>::iterator start1;
		std::vector<std::vector<std::string>>::iterator start2;

		Initialize(name_of_first_table, name_of_second_table, first_column, second_column,
		           first_table, second_table,
		           number_of_first_column_to_check_pred, number_of_second_column_to_check_pred, start1, start2);

		for (auto it1 = start1; it1 != first_table.table_.end(); ++it1) {
			std::vector<std::string> what_to_add_to_table;

			for (const auto& i : columns) {
				auto is_In_first = FindColumnIndexForJoins(
					i, (*(it1->begin() + number_of_first_column_to_check_pred)),
					number_of_first_column_to_check_pred, first_table);
				auto is_In_second = FindColumnIndexForJoins(
					i, (*(it1->begin() + number_of_first_column_to_check_pred)),
					number_of_second_column_to_check_pred, second_table);

				if (is_In_first != -1) {
					what_to_add_to_table.emplace_back(it1->begin()[is_In_first]);
				}

				else if (is_In_second != -1) {
					for (auto it2 = start2; it2 != second_table.table_.end(); ++it2) {
						if (pred.lambda_(*(it1->begin() + number_of_first_column_to_check_pred),
						                 *(it2->begin() + number_of_second_column_to_check_pred))) {
							what_to_add_to_table.emplace_back(it2->begin()[is_In_second]);
						}
					}
				}

				else {
					what_to_add_to_table.emplace_back("NULL");
				}
			}

			Insert(name_of_new_table, what_to_add_to_table);
		}
	}


	template <typename lambda, typename... Strings>
	void RightJoin(const std::string& name_of_first_table, const std::string& name_of_second_table,
	               const std::string& name_of_new_table,
	               const std::string& primary_key, On<lambda> pred,
	               const std::string& first_column,
	               const std::string& second_column, Strings&&... names_of_columns) {
		DataBase::CreateTable(name_of_new_table, primary_key, names_of_columns...);
		std::vector<std::string> columns{names_of_columns...};

		Table first_table;
		Table second_table;
		size_t number_of_first_column_to_check_pred;
		size_t number_of_second_column_to_check_pred;

		std::vector<std::vector<std::string>>::iterator start1;
		std::vector<std::vector<std::string>>::iterator start2;

		Initialize(name_of_first_table, name_of_second_table, first_column, second_column,
		           first_table, second_table,
		           number_of_first_column_to_check_pred, number_of_second_column_to_check_pred, start1, start2);


		for (auto it2 = start2; it2 != second_table.table_.end(); ++it2) {
			std::vector<std::string> what_to_add_to_table;

			for (const auto& i : columns) {
				auto is_In_first = FindColumnIndexForJoins(
					i, (*(it2->begin() + number_of_second_column_to_check_pred)),
					number_of_first_column_to_check_pred, first_table);

				auto is_In_second = FindColumnIndexForJoins(
					i, (*(it2->begin() + number_of_second_column_to_check_pred)),
					number_of_second_column_to_check_pred, second_table);


				if (is_In_first != -1) {
					for (auto it1 = start1; it1 != first_table.table_.end(); ++it1) {
						if (pred.lambda_(*(it1->begin() + number_of_first_column_to_check_pred),
						                 *(it2->begin() + number_of_second_column_to_check_pred))) {
							what_to_add_to_table.emplace_back(it1->begin()[is_In_first]);
							break;
						}
					}
				}

				else if (is_In_second != -1) {
					what_to_add_to_table.emplace_back(it2->begin()[is_In_second]);
				}


				else {
					what_to_add_to_table.emplace_back("NULL");
				}
			}


			Insert(name_of_new_table, what_to_add_to_table);
		}
	}


	friend std::ostream& operator <<(std::ostream& os, const DataBase& data_base);


	void Parce(const std::string& path) {
		std::ifstream infile(path);

		std::string input((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));

		std::stringstream ss_(input);
		std::vector<std::string> requests;
		std::string line;

		while (std::getline(ss_, line, ';')) {
			requests.push_back(line);
		}


		for (const auto& request : requests) {
			std::stringstream ss(request);
			std::string command;
			ss >> command;

			if (command == "CREATE_TABLE") {
				std::string table_name;
				std::string name_of_column_with_primary_keys;
				std::vector<std::string> columns;


				std::getline(ss, table_name, ',');
				std::getline(ss, name_of_column_with_primary_keys, ',');

				EraseSpaces(table_name);
				EraseSpaces(name_of_column_with_primary_keys);

				std::string column_name, column_type;

				while (std::getline(ss, column_name, ',')) {
					std::getline(ss, column_type, ',');

					EraseSpaces(column_name);
					columns.emplace_back(column_name);
				}

				CreateTable(table_name, name_of_column_with_primary_keys, columns);
			}
			else if (command == "INSERT") {
				std::string table_name;
				std::vector<std::string> values;


				std::getline(ss, table_name, ',');


				EraseSpaces(table_name);


				std::string value;
				while (std::getline(ss, value, ',')) {
					EraseSpaces(value);
					values.emplace_back(value);
				}

				Insert(table_name, values);
			}

			else if (command == "UPDATE") {
				std::string table_name;
				std::string column_name;
				std::string replacement;

				ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');

				std::string remaining;
				std::getline(ss, remaining);

				EraseSpaces(remaining);


				std::stringstream tmp_ss(remaining);

				std::getline(tmp_ss, table_name, ',');
				std::getline(tmp_ss, column_name, ',');
				std::getline(tmp_ss, replacement, ',');

				Update(table_name, column_name, replacement);
			}

			else if (command == "DROP_TABLE") {
				std::string table_name;

				std::getline(ss, table_name, ',');
				EraseSpaces(table_name);

				DropTable(table_name);
			}

			else if (command == "DELETE") {
				std::string table_name;

				std::getline(ss, table_name, ',');
				EraseSpaces(table_name);

				Delete(table_name);
			}
		}
	}


	template <typename lambda, typename... Colums>
	Table Select(const std::string& from, const On<lambda> where, const std::string& predicate_column,
	             Colums&&... colums) {
		size_t index_of_table = FindNumberOfTable(from);
		std::vector<std::string> columns{colums...};

		if (columns.size() > 1 and std::ranges::find(columns, "*") != columns.end()) {
			throw std::invalid_argument("* with some values. What am I supposted to do?");
		}


		auto cur_table = tables[FindNumberOfTable(from)];

		auto tmp = cur_table.names_of_columns_;


		auto table = Table(random_string(10), cur_table.names_of_columns_[0], tmp);

		size_t index_for_pred = FindColumnIndex(predicate_column, cur_table);

		for (auto start = tables[index_of_table].table_.begin(); start != tables[index_of_table].table_.end(); ++
		     start) {
			auto start1 = start->begin();

			std::vector<std::string> what_to_add_to_table;


			if (where.lambda_(*(start1 + index_for_pred))) {
				for (const auto& i : table.names_of_columns_) {
					auto idx = FindColumnIndex(i, tables[index_of_table]);

					what_to_add_to_table.emplace_back(*(start1 + idx));
				}
				table.AddRow(what_to_add_to_table);
			}
		}

		return table;
	}

	template <typename... Colums>
	[[nodiscard]] Table Select(const std::string& from, Colums&&... colums) {
		size_t index_of_table = FindNumberOfTable(from);
		std::vector<std::string> columns{colums...};

		if (columns.size() > 1 and std::ranges::find(columns, "*") != columns.end()) {
			throw std::invalid_argument("* with some values. What am I supposted to do?");
		}

		if (columns.size() == 1 and columns[0] == "*") {
			return tables[index_of_table];
		}


		auto table = Table(random_string(10), columns[0], colums...);

		for (auto start = tables[index_of_table].table_.begin(); start != tables[index_of_table].table_.end(); ++
		     start) {
			auto start1 = start->begin();

			std::vector<std::string> what_to_add_to_table;


			for (const auto& i : columns) {
				auto idx = FindColumnIndex(i, tables[index_of_table]);

				what_to_add_to_table.emplace_back(*(start1 + idx));
			}


			table.AddRow(what_to_add_to_table);
		}

		return table;
	}


	[[nodiscard]] size_t Size() const {
		return tables.size();
	}


	[[nodiscard]] const Table& GetTable(size_t idx) const {
		return tables[idx];
	}

private:
	std::vector<Table> tables;

	size_t GetIndex(const std::string& table_name) {
		size_t idx = 0;

		for (decltype(auto) i : tables) {
			if (i.name_of_table_ == table_name) {
				break;
			}
			++idx;
		}

		return idx;
	}


	void Initialize(const std::string& first_table_to_join, const std::string& second_table_to_join,
	                const std::string& first_column_to_check, const std::string& second_column_to_check,
	                Table& first_table,
	                Table& second_table, size_t& number_of_first_column_to_check_pred,
	                size_t& number_of_second_column_to_check_pred,
	                std::vector<std::vector<std::string>>::iterator& start1,
	                std::vector<std::vector<std::string>>::iterator& start2) {
		first_table = tables[FindNumberOfTable(first_table_to_join)];
		second_table = tables[FindNumberOfTable(second_table_to_join)];


		number_of_first_column_to_check_pred = FindColumnIndex(first_column_to_check, first_table);

		number_of_second_column_to_check_pred = FindColumnIndex(second_column_to_check, second_table);

		start1 = first_table.table_.begin();
		start2 = second_table.table_.begin();
	}


	std::string random_string(const size_t len) {
		static constexpr char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		std::string s;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

		for (size_t i = 0; i < len; ++i) {
			s += alphanum[dis(gen)];
		}
		return s;
	}

	[[nodiscard]] int64_t FindColumnIndex(const std::string& name_of_column, const Table& cur_table) {
		int64_t idx = 0;

		for (decltype(auto) names_of_columns : cur_table.names_of_columns_) {
			if (names_of_columns == name_of_column) {
				return idx;
			}

			++idx;
		}

		return -1;
	}

	[[nodiscard]] int64_t FindColumnIndexForJoins(const std::string& name_of_column, const std::string& what_to_find,
	                                              size_t index_for_find, const Table& cur_table) {
		int64_t idx = 0;

		for (decltype(auto) names_of_columns : cur_table.names_of_columns_) {
			if (names_of_columns == name_of_column) {
				break;
			}
			++idx;
		}

		if (idx == cur_table.names_of_columns_.size()) {
			return -1;
		}

		for (decltype(auto) jesus : cur_table.table_) {
			if (jesus[index_for_find] == what_to_find) {
				return idx;
			}
		}


		return -1;
	}


	void EraseSpaces(std::string& str) const {
		std::erase_if(str, [](char c) {
			return std::isspace(static_cast<unsigned char>(c));
		});
	}


	[[nodiscard]] size_t FindNumberOfTable(const std::string& name_of_table) const {
		size_t idx = 0;
		for (decltype(auto) table : tables) {
			if (table.name_of_table_ == name_of_table)
				return idx;
			++idx;
		}
		throw std::invalid_argument("No such table present");
	}
};


inline std::ostream& operator <<(std::ostream& os, const DataBase& data_base) {
	for (auto& all_tables : data_base.tables) {
		for (auto& k : all_tables.table_) {
			for (const auto& values : k) {
				os << values << " ";
			}
			os << "\n";
		}


		os << "\n\n";
	}

	return os;
}
