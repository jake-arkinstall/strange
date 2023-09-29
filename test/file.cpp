#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <concepts>
#include <filesystem>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <strange/strange.h>

TEST_CASE("Text file write and read"){
    auto temporary_filename = std::filesystem::temp_directory_path() / "strange_file_test.txt";
    {
        // try to open the file and check it before it can be fed into the pipeline
        auto maybe_writer = strange::text_file_writer::try_open(temporary_filename);
        REQUIRE(maybe_writer.has_value());

        // for each line, write it to the file
        auto write_pipeline = strange::builder{}
                            | strange::each{
                                "This is line 1 of the file",
                                "This is line 2 of the file",
                                "This is line 3 of the file"
                            }
                            | maybe_writer.value()
                            | strange::swallow{};
        write_pipeline();
    }
    {
        auto maybe_reader = strange::text_file_reader<128>::try_open(temporary_filename);
        REQUIRE(maybe_reader.has_value());
        std::vector<std::string> lines;
        auto read_pipeline = strange::builder{}
                           | maybe_reader.value()
                           | strange::to_vector{lines};
        read_pipeline();

        REQUIRE(lines.size() == 3);
        REQUIRE(lines[0] == "This is line 1 of the file\n");
        REQUIRE(lines[1] == "This is line 2 of the file\n");
        REQUIRE(lines[2] == "This is line 3 of the file\n");
    }
}
