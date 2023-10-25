#include "Gng2D/core/luna.hpp"
#include "gtest/gtest.h"

static std::string TEST_DIRECTORY()
{
    std::string s{__FILE__};
    auto cutFrom = s.find_last_of('/');
    return s.substr(0, cutFrom+1);
}

struct LunaTest : ::testing::Test 
{
    inline static std::string testFile{TEST_DIRECTORY() + "test.lua"};

    Gng2D::Luna luna;
};

TEST_F(LunaTest, AfterDoingFile_LunaCanReadGlobalVariables)
{
    luna.doFile(testFile);

    ASSERT_EQ(*luna.readGlobalInt("TEST_INT"), 123);
    ASSERT_EQ(*luna.readGlobalFloat("TEST_FLOAT"), 123.321);
    ASSERT_EQ(*luna.readGlobalBool("TEST_BOOL"), false);
    ASSERT_EQ(*luna.readGlobalString("TEST_STRING"), "HELLO LUA!");
}

TEST_F(LunaTest, AfterDoingFile_LunaCanUseReadToVar)
{
    luna.doFile(testFile);

    int         testInt;
    double      testFloat;
    bool        testBool;
    std::string testString;

    luna.readToVar("TEST_INT", testInt);
    luna.readToVar("TEST_FLOAT", testFloat);
    luna.readToVar("TEST_BOOL", testBool);
    luna.readToVar("TEST_STRING", testString);

    ASSERT_EQ(testInt, 123);
    ASSERT_EQ(testFloat, 123.321);
    ASSERT_EQ(testBool, false);
    ASSERT_EQ(testString, "HELLO LUA!");
}

