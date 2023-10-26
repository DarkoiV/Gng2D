#include "Gng2D/core/luna.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>

constexpr char INT_ID[]         = "TEST_INT";
constexpr int INT_VALUE         = 129;
constexpr char FLOAT_ID[]       = "TEST_FLOAT";
constexpr double FLOAT_VALUE    = 129.389;
constexpr char BOOL_ID[]        = "TEST_BOOL";
constexpr bool BOOL_VALUE       = false;
constexpr char STR_ID[]         = "TEST_STRING";
constexpr char STR_VALUE[]      = "HELLO LUNA!";

struct LunaTest : ::testing::Test 
{
    inline static std::string testFilesDir{TEST_FILES_DIR};
    inline static std::string testFileName{"test.lua"};
    inline static std::filesystem::path testFile{testFilesDir + "/" + testFileName};
    static void SetUpTestSuite()
    {
        std::fstream file(testFile, std::fstream::out);
        std::boolalpha(file);
        file << INT_ID      << " = " << INT_VALUE   << "\n";
        file << FLOAT_ID    << " = " << FLOAT_VALUE << "\n";
        file << BOOL_ID     << " = " << BOOL_VALUE  << "\n";
        file << STR_ID      << " = " << std::quoted(STR_VALUE) << std::endl;
    }

    Gng2D::Luna luna;
};

TEST_F(LunaTest, AfterDoingFile_LunaCanReadGlobalVariables)
{
    luna.doFile(testFile.string());

    auto readInt    = luna.readGlobalInt(INT_ID);
    auto readFloat  = luna.readGlobalFloat(FLOAT_ID);
    auto readBool   = luna.readGlobalBool(BOOL_ID);
    auto readString = luna.readGlobalString(STR_ID);

    ASSERT_TRUE(readInt);
    ASSERT_TRUE(readFloat);
    ASSERT_TRUE(readBool);
    ASSERT_TRUE(readString);

    ASSERT_EQ(*readInt, INT_VALUE);
    ASSERT_EQ(*readFloat, FLOAT_VALUE);
    ASSERT_EQ(*readBool, BOOL_VALUE);
    ASSERT_EQ(*readString, STR_VALUE);
}

TEST_F(LunaTest, AfterDoingFile_LunaCanUseReadToVar)
{
    luna.doFile(testFile);

    int         testInt;
    double      testFloat;
    bool        testBool;
    std::string testString;

    luna.readToVar(INT_ID, testInt);
    luna.readToVar(FLOAT_ID, testFloat);
    luna.readToVar(BOOL_ID, testBool);
    luna.readToVar(STR_ID, testString);

    ASSERT_EQ(testInt, INT_VALUE);
    ASSERT_EQ(testFloat, FLOAT_VALUE);
    ASSERT_EQ(testBool, BOOL_VALUE);
    ASSERT_EQ(testString, STR_VALUE);
}

