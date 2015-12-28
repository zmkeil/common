#include <iostream>
#include <gtest/gtest.h>
#include "string_printf.h"

int main(int argc, char** argv)
{
    /*example*/
    std::string str;
    str.reserve(2048);
    common::string_appendf(&str, "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello");
    common::string_appendn(&str, "\nworld\nworld", 6);
    std::cout << str << std::endl;

    /*cases*/
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(StringPrintfTest, test_string_appendf)
{
    std::string str;
    str.reserve(10);

    common::string_appendf(&str, "hellohello");
    // the last be seted '\0' by vsnprintf
    ASSERT_STREQ("hellohell", str.c_str());
    // and output.resize(write_point + remain -1)
    ASSERT_EQ(9, str.size());

    str.reserve(50);
    common::string_appendf(&str, "worldworld");
    ASSERT_STREQ("hellohellworldworld", str.c_str());
    ASSERT_EQ(19, str.size());
}

TEST(StringPrintfTest, test_string_appendn)
{
    std::string str;
    common::string_appendn(&str, "hello", 5);
    ASSERT_EQ(256/*STRING_APPEND_MIN_SIZE*/, str.capacity());
    ASSERT_EQ(5, str.size());
    ASSERT_STREQ("hello", str.c_str());

    std::string str1;
    str1.reserve(5);
    common::string_appendn(&str1, "hello", 5);
    ASSERT_EQ(5, str1.size());
    ASSERT_STREQ("hello", str1.c_str());
}
