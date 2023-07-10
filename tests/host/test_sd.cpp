// #include "traces.h"
#include <cassert>
#include <stdio.h>
#include <pico/stdlib.h>
// #include "bc_test.h"
#include "pico_test.hpp"
#include "traces.h"


#include <iostream>

#include "ff.h"
#include "sd_file.h"
#include "sd_common.h"
#include "session.hpp"

#define TEST_FOLDER SD_TEST_FOLDER "/tests"


TEST_CASE_SUB_FUNCTION_DEF(sd_drive_test)
{
    const char* file_name = TEST_FOLDER "/sd_test.csv";
    mount_drive();
    // f_unlink(file_name);
    Sd_File file(file_name);
    PRINTF("File opened\n");
    file.clear();
    PRINTF("File cleared\n");

    const char* header_line = "time_start;time_end;duration;velocity_max;velocity_avg;distance\n";
    const char* first_line = "16720.83.83,69:68:6.56e;2022.08.09,20:54:13.01;01:00:50.00;30.0000;20.0000;13110\n";
    PRINTF("header_line: %s\n", header_line);
    PRINTF("first_line: %s\n", first_line);

    if(file.is_empty())
    {
        file.append(header_line);
    }
    file.append(first_line);
    {
        const size_t file_size = strlen(header_line) + strlen(first_line);
        const size_t get_size = file.get_size();
        PICO_TEST_CHECK_EQ(get_size, file_size);
    }
    PRINTF("File written\n");

    FIL fp;
    auto res = f_open(&fp, file_name, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    PICO_TEST_ASSERT_VERBAL(res == FR_OK, "Open result = %d", res);

    //res = f_truncate(&fp);
    res = f_rewind(&fp);
    PICO_TEST_ASSERT_VERBAL(res == FR_OK, "Rewind file result = %d", res);

    FILINFO info;
    f_stat(file_name, &info);
    uint64_t file_size = info.fsize;

    PICO_TEST_ASSERT_VERBAL(file_size > 0, "File %s size is %" PRIu64, file_name, file_size);
    {
        auto test_string = header_line;
        enum{BUFFER_SIZE=256};
        char buffer[BUFFER_SIZE] = {0};
        UINT bytes_read = 0;
        PRINTF("Reading\n");
        auto res = f_read(&fp, buffer, strlen(test_string), &bytes_read);
        PRINTF("Reading ok\n");
        PICO_TEST_ASSERT_VERBAL(res == FR_OK, "%d", res);
        PRINTF(" read: %s\n", buffer);
        PICO_TEST_CHECK_VERBAL(bytes_read == strlen(test_string), "%d", bytes_read);
        PICO_TEST_ASSERT_VERBAL(strcmp(test_string, buffer) == 0, "test_string='%s'\tbuffer='%s'", test_string, buffer);
    }

    {
        auto test_string = first_line;
        enum{BUFFER_SIZE=256};
        char buffer[BUFFER_SIZE] = {0};
       // auto res = f_gets(buffer, BUFFER_SIZE, &fp);
        // PICO_TEST_ASSERT_VERBAL(res != NULL, "%d", res);
        // printf(" read: %s\n", res);
        // PICO_TEST_ASSERT(strcmp(test_string, res) == 0);
        UINT bytes_read = 0;
        PRINTF("Reading\n");
        auto res = f_read(&fp, buffer, strlen(test_string), &bytes_read);
        PICO_TEST_ASSERT_VERBAL(res == FR_OK, "%d", res);
        PICO_TEST_ASSERT_VERBAL(bytes_read == strlen(test_string), "%d", bytes_read);
        PRINTF("Reading ok\n");
        //PRINTF(" read: %s\n", buffer);
        PICO_TEST_ASSERT(strcmp(test_string, buffer) == 0);
        PRINTF("Reading ok2\n");

    }
    f_close(&fp);
    PRINTF("Close\n");

    const std::string file_content_expected = std::string(header_line) + std::string(first_line);
    const std::string file_content = file.read_all();
    PICO_TEST_CHECK_EQ(file_content_expected, file_content);

    file.clear();
    {
        const size_t file_size = 0;
        const size_t get_size = file.get_size();
        PICO_TEST_CHECK_EQ(get_size, file_size);
    }
    {
        const std::string content = "1234567890";
        file.append(content.c_str());
        {
            const size_t file_size = content.length();
            const size_t get_size = file.get_size();
            PICO_TEST_CHECK_EQ(get_size, file_size);
        }

        const std::string second_content = "0987654321";
        file.append(second_content.c_str());
        // file = content second_content
        PICO_TEST_ASSERT(content.length() == second_content.length());


        {
            const std::string file_content_expected = content + second_content;
            const std::string file_content = file.read_all();
            PICO_TEST_CHECK_EQ(file_content_expected, file_content);
        }
        file.overwrite(second_content.c_str(), 0);
        {
            const std::string file_content_expected = second_content + second_content;
            const std::string file_content = file.read_all();
            PICO_TEST_CHECK_EQ(file_content_expected, file_content);
        }
        file.overwrite(content.c_str(), second_content.length());
        {
            const std::string file_content_expected = second_content + content;
            const std::string file_content = file.read_all();
            PICO_TEST_CHECK_EQ(file_content_expected, file_content);
        }
    }

    file.remove();
}

TEST_CASE_SUB_FUNCTION_DEF(list_dir_test)
{
    #define TEST_FOLDER_DIR_TESTS TEST_FOLDER "/dir_tests/"
    #define TEST_FILE_1 "f1.txt"
    f_mkdir(TEST_FOLDER_DIR_TESTS);
    dir::del(TEST_FOLDER_DIR_TESTS);
    f_mkdir(TEST_FOLDER_DIR_TESTS);


    Sd_File file(TEST_FOLDER_DIR_TESTS TEST_FILE_1);
    file.clear();

    // check if file exist and can read/write
    auto f1_content = "123456";
    file.append(f1_content);
    auto f1_content_read = file.read_all();
    PICO_TEST_CHECK_VERBAL(f1_content_read.compare(f1_content) == 0,
                    "f1 content %s", f1_content_read.c_str());


    // check if get files return created file
    auto files_in_folder = dir::get_files(TEST_FOLDER_DIR_TESTS);
    PICO_TEST_CHECK_VERBAL(files_in_folder.size() == 1, "no files: %zu", files_in_folder.size());
    PICO_TEST_CHECK_VERBAL(files_in_folder.at(0).compare(TEST_FILE_1) == 0, "f(0): %s", files_in_folder.at(0).c_str());

    dir::del(TEST_FOLDER_DIR_TESTS);

    Sd_File file2(TEST_FOLDER "/f2.cfg");


    // cleanup

    // for test file gen
    // {
    //     f_mkdir("history");
    //     Sd_File file0("history/h0.log");
    //     Sd_File file1("history/h1.log");
    //     Sd_File file2("history/h2.log");
    //     Sd_File file3("history/h3.log");
    //     Sd_File file4("history/h4.log");
    // }
}

TEST_CASE_SUB_FUNCTION_DEF(line_access_test)
{
    Sd_File test_file(TEST_FOLDER "/__test__.txt");
    test_file.clear();
    test_file.append("1.line\n");
    test_file.append("2.line\n");
    test_file.append("3.line01234567890123456789012345678901234567890123456789012345678901234567890123456789\n");
    test_file.append("4.line01234567890123456789012345678901234567890123456789012345678901234567890123456789\n");
    test_file.append("5.line\n");
    test_file.append("6.line");

    auto ll = test_file.read_last_line(10);
    std::cout << "ll:" << ll << std::endl;
    PICO_TEST_ASSERT_VERBAL(ll.compare("6.line") == 0, "%s == %s", ll.c_str(), "6.line");

    std::cout << "#####################################################" << std::endl;


    size_t expected_on_lines = 6;
    auto no_lines = test_file.get_no_of_lines();
    PICO_TEST_CHECK_EQ(expected_on_lines, no_lines);

    auto l0 = test_file.read_line(0, 128);
    {
        const char* expected = "1.line";
        std::cout <<"\n> l0:" << l0 << std::endl;
        PICO_TEST_CHECK_VERBAL(l0.compare(expected) == 0, "%s!=%s", l0.c_str(), expected);
    }

    auto l4 = test_file.read_line(4, 128);
    {
        const char* expected = "5.line";
        std::cout << "\n> l4:" << l4 << std::endl;
        PICO_TEST_CHECK_VERBAL(l4.compare(expected) == 0, "%s!=%s", l4.c_str(), expected);
    }

    auto l5 = test_file.read_line(5, 128);
    {
        const char* expected = "6.line";
        std::cout << "\n> l5:" << l5 << std::endl;
        PICO_TEST_CHECK_VERBAL(l5.compare(expected) == 0, "%s!=%s", l5.c_str(), expected);
    }

    auto l3 = test_file.read_line(3, 16);
    {
        const char* expected = "4.line0123456789";
        std::cout << "\n> l3:" << l3 << std::endl;
        PICO_TEST_CHECK_VERBAL(l3.compare(expected) == 0, "%s!=%s", l3.c_str(), expected);
    }
    // PICO_TEST_CHECK(test_file.read_line(4, 128).compare("5.line") == 0);
    // PICO_TEST_CHECK(test_file.read_line(5, 128).compare("6.line") == 0);

    test_file.remove();
}


TEST_CASE_FUNCTION(tc_sd_basic)
{
    f_mkdir(TEST_FOLDER);
    TEST_CASE_SUB_FUNCTION(sd_drive_test);
    TEST_CASE_SUB_FUNCTION(list_dir_test);
    TEST_CASE_SUB_FUNCTION(line_access_test);

    PICO_TEST_ASSERT(1 == 1);
}

int main()
{
    traces_init();
    return tc_sd_basic().asserts_failed;
}
