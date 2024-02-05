#include <lib/DataBase.h>
#include <gtest/gtest.h>

TEST(DataBaseTests, DefaultTest) {
	DataBase data_base;

	data_base.CreateTable("branch", "branch_id", "branch_id", "branch_name", "mgr_id", "mgr_start_date");
	ASSERT_EQ(data_base.Size(), 1);

	ASSERT_EQ(data_base.GetTable(0).GetAmountOfRows(), 0);

	data_base.Insert("branch", 1, "Corporate", "100", "2006");
	data_base.Insert("branch", 2, "Scranton", "102", "1992");

	ASSERT_EQ(data_base.GetTable(0).GetAmountOfRows(), 2);

	data_base.Insert("branch", 3, "Stamford", "106", "1998");
	data_base.Insert("branch", 4, "Buffalo");

	ASSERT_EQ(data_base.GetTable(0).GetAmountOfColumns(), 4);

	ASSERT_EQ(data_base.GetTable(0).GetAmountOfRows(), 4);


	data_base.CreateTable("employee", "emp_id", "emp_id", "first_name", "last_name", "birth_date");

	ASSERT_EQ(data_base.Size(), 2);
	ASSERT_EQ(data_base.GetTable(1).GetAmountOfRows(), 0);

	data_base.Insert("employee", 100, "David", "Wallace", "1967");

	data_base.Insert("employee", 101, "Jan", "Levinson", "1961");
	data_base.Insert("employee", 102, "Michael", "Scott", "1964");
	data_base.Insert("employee", 103, "Angela", "Martin", "1971");
	data_base.Insert("employee", 104, "Kelly", "Kapoor", "1980");
	data_base.Insert("employee", 105, "Starley", "Hudson", "1958");
	ASSERT_EQ(data_base.GetTable(1).GetAmountOfRows(), 6);
	data_base.Insert("employee", 106, "Josh", "Porter", "1969");

	std::vector<std::string> six_row{"106", "Josh", "Porter", "1969"};

	ASSERT_EQ(data_base.GetTable(1).GetRow(6), six_row);

	data_base.Insert("employee", 107, "Andy", "Bernand", "1973");
	data_base.Insert("employee", 108, "Jim", "Haipert", "1978");
}

TEST(DataBaseTests, UpdateTest) {
	DataBase data_base;

	EXPECT_DEATH(data_base.GetTable(0), ".*");


	data_base.CreateTable("students", "student_id", "student_id", "name", "surname", "scores", "studying");

	data_base.Insert("students", 100, "David", "Wallace", "0", "ITMO");
	data_base.Insert("students", 101, "Jan", "Levinson", "10", "ITMO");
	data_base.Insert("students", 102, "Michael", "Scott", "29", "ITMO");
	data_base.Insert("students", 103, "Angela", "Martin", "17", "SPGU");
	data_base.Insert("students", 104, "Kelly", "Kapoor", "60", "ITMO");
	data_base.Insert("students", 105, "Starley", "Hudson", "100", "SPGU");
	data_base.Insert("students", 106, "Josh", "Porter", "103", "SPGU");
	data_base.Insert("students", 107, "Andy", "Bernand", "55", "LETI");
	data_base.Insert("students", 108, "Jim", "Haipert", "34", "LETI");


	auto lambda = [](std::string& str1) {
		return stoi(str1) < 60;
	};

	On o_(lambda);

	data_base.Update("students", "studying", "Army", "scores", o_);

	ASSERT_EQ(data_base.GetTable(0).GetRow(0)[4], "Army");
	ASSERT_EQ(data_base.GetTable(0).GetRow(6)[4], "SPGU");


	data_base.Update("students", "studying", "May the piece be forever in  our time");

	ASSERT_EQ(data_base.GetTable(0).GetRow(0)[4], "May the piece be forever in  our time");
	ASSERT_EQ(data_base.GetTable(0).GetRow(6)[4], "May the piece be forever in  our time");
}

TEST(DataBaseTests, DeleteDropTest) {
	DataBase data_base;

	data_base.CreateTable("students", "student_id", "student_id", "name", "surname", "scores", "studying");

	data_base.Insert("students", 100, "David", "Wallace", "0", "ITMO");
	data_base.Insert("students", 101, "Jan", "Levinson", "10", "ITMO");
	data_base.Insert("students", 102, "Michael", "Scott", "29", "ITMO");
	data_base.Insert("students", 103, "Angela", "Martin", "17", "SPGU");
	data_base.Insert("students", 104, "Kelly", "Kapoor", "60", "ITMO");
	data_base.Insert("students", 105, "Starley", "Hudson", "100", "SPGU");
	data_base.Insert("students", 106, "Josh", "Porter", "103", "SPGU");
	data_base.Insert("students", 107, "Andy", "Bernand", "55", "LETI");
	data_base.Insert("students", 108, "Jim", "Haipert", "34", "LETI");


	data_base.CreateTable("branch", "branch_id", "branch_id", "branch_name", "mgr_id", "mgr_start_date");

	data_base.Insert("branch", 1, "Corporate", "100", "2006");
	data_base.Insert("branch", 2, "Scranton", "102", "1992");
	data_base.Insert("branch", 3, "Stamford", "106", "1998");

	data_base.Insert("branch", 4, "Buffalo");

	data_base.CreateTable("employee", "emp_id", "emp_id", "first_name", "last_name", "birth_date");

	data_base.Insert("employee", 100, "David", "Wallace", "1967");

	data_base.Insert("employee", 101, "Jan", "Levinson", "1961");
	data_base.Insert("employee", 102, "Michael", "Scott", "1964");
	data_base.Insert("employee", 103, "Angela", "Martin", "1971");
	data_base.Insert("employee", 104, "Kelly", "Kapoor", "1980");
	data_base.Insert("employee", 105, "Starley", "Hudson", "1958");
	data_base.Insert("employee", 106, "Josh", "Porter", "1969");
	data_base.Insert("employee", 107, "Andy", "Bernand", "1973");
	data_base.Insert("employee", 108, "Jim", "Haipert", "1978");


	auto lambda = [](std::string& str) {
		return stoi(str) >= 105;
	};

	Where o_(lambda);


	data_base.Delete("employee", "emp_id", o_);

	ASSERT_EQ(data_base.GetTable(2).GetAmountOfRows(), 5);

	ASSERT_EQ(data_base.Size(), 3);

	data_base.Delete("employee");

	ASSERT_EQ(data_base.GetTable(2).GetAmountOfRows(), 0);

	ASSERT_EQ(data_base.GetTable(2).GetAmountOfColumns(), 4);

	data_base.DropTable("employee");
	ASSERT_EQ(data_base.Size(), 2);

	data_base.DropTable("branch");
	data_base.DropTable("students");
	ASSERT_EQ(data_base.Size(), 0);

	ASSERT_ANY_THROW(data_base.Delete("employee123"));
	ASSERT_ANY_THROW(data_base.DropTable("employee123"));
}


TEST(DataBaseTests, Joins) {
	DataBase data_base;

	data_base.CreateTable("employee", "emp_id", "emp_id", "first_name", "last_name", "birth_date");

	data_base.Insert("employee", 100, "David", "Wallace", "1967");

	data_base.Insert("employee", 101, "Jan", "Levinson", "1961");
	data_base.Insert("employee", 102, "Michael", "Scott", "1964");
	data_base.Insert("employee", 103, "Angela", "Martin", "1971");
	data_base.Insert("employee", 104, "Kelly", "Kapoor", "1980");
	data_base.Insert("employee", 105, "Starley", "Hudson", "1958");
	data_base.Insert("employee", 106, "Josh", "Porter", "1969");
	data_base.Insert("employee", 107, "Andy", "Bernand", "1973");
	data_base.Insert("employee", 108, "Jim", "Haipert", "1978");

	data_base.CreateTable("branch", "branch_id", "branch_id", "branch_name", "mgr_id", "mgr_start_date");

	data_base.Insert("branch", 1, "Corporate", "100", "2006");
	data_base.Insert("branch", 2, "Scranton", "102", "1992");
	data_base.Insert("branch", 3, "Stamford", "106", "1998");
	data_base.Insert("branch", 4, "Buffalo", "NULL", "NULL");
	auto lambda = [](std::string& str, std::string& str1) {
		return str == str1;
	};
	On predicate(lambda);

	data_base.InnerJoin("employee", "branch", "inner_joined_table",
	                    "first_name", predicate, "emp_id",
	                    "mgr_id", "emp_id", "first_name", "branch_name");


	ASSERT_EQ(data_base.Size(), 3);

	ASSERT_EQ(data_base.GetTable(2).GetAmountOfRows(), 3);
	ASSERT_EQ(data_base.GetTable(2).GetAmountOfColumns(), 3);

	std::vector<std::string> first_row{"100", "David", "Corporate"};
	std::vector<std::string> second_row{"102", "Michael", "Scranton"};
	std::vector<std::string> third_row{"106", "Josh", "Stamford"};
	std::vector<std::string> last_row{"108", "Jim", "NULL"};
	std::vector<std::string> last_right_row{"NULL", "NULL", "Buffalo"};
	ASSERT_EQ(data_base.GetTable(2).GetRow(0), first_row);
	ASSERT_EQ(data_base.GetTable(2).GetRow(1), second_row);
	ASSERT_EQ(data_base.GetTable(2).GetRow(2), third_row);


	data_base.LeftJoin("employee", "branch", "left_joined_table",
	                   "first_name", predicate, "emp_id",
	                   "mgr_id", "emp_id", "first_name", "branch_name");


	ASSERT_EQ(data_base.Size(), 4);

	ASSERT_EQ(data_base.GetTable(3).GetAmountOfRows(), 9);
	ASSERT_EQ(data_base.GetTable(3).GetAmountOfColumns(), 3);

	ASSERT_EQ(data_base.GetTable(3).GetRow(0), first_row);
	ASSERT_EQ(data_base.GetTable(3).GetRow(2), second_row);
	ASSERT_EQ(data_base.GetTable(3).GetRow(6), third_row);

	ASSERT_EQ(data_base.GetTable(3).GetRow(8), last_row);


	data_base.RightJoin("employee", "branch", "right_joined_table",
	                    "first_name", predicate, "emp_id",
	                    "mgr_id", "emp_id", "first_name", "branch_name");

	ASSERT_EQ(data_base.Size(), 5);

	ASSERT_EQ(data_base.GetTable(4).GetAmountOfRows(), 4);
	ASSERT_EQ(data_base.GetTable(4).GetAmountOfColumns(), 3);

	ASSERT_EQ(data_base.GetTable(4).GetRow(0), first_row);
	ASSERT_EQ(data_base.GetTable(4).GetRow(1), second_row);
	ASSERT_EQ(data_base.GetTable(4).GetRow(2), third_row);

	ASSERT_EQ(data_base.GetTable(4).GetRow(3), last_right_row);
}
