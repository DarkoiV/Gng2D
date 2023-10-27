#include "Gng2D/core/luna.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>
#include <sstream>

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

    auto readInt    = luna.readInt(INT_ID);
    auto readFloat  = luna.readFloat(FLOAT_ID);
    auto readBool   = luna.readBool(BOOL_ID);
    auto readString = luna.readString(STR_ID);

    ASSERT_TRUE(readInt);
    ASSERT_TRUE(readFloat);
    ASSERT_TRUE(readBool);
    ASSERT_TRUE(readString);

    ASSERT_EQ(*readInt, INT_VALUE);
    ASSERT_EQ(*readFloat, FLOAT_VALUE);
    ASSERT_EQ(*readBool, BOOL_VALUE);
    ASSERT_EQ(*readString, STR_VALUE);
}

TEST_F(LunaTest, WhenReadingNotDefinedGlobal_LunaWillReturnNullopt)
{
    ASSERT_EQ(luna.readInt("NOT_DEF_1"), std::nullopt);
    ASSERT_EQ(luna.readFloat("NOT_DEF_2"), std::nullopt);
    ASSERT_EQ(luna.readBool("NOT_DEF_3"), std::nullopt);
    ASSERT_EQ(luna.readString("NOT_DEF_4"), std::nullopt);
}

TEST_F(LunaTest, AfterDoingFile_LunaCanUseReadToVar)
{
    luna.doFile(testFile);

    int         testInt;
    double      testFloat;
    bool        testBool;
    std::string testString;

    ASSERT_TRUE(luna.readToVar(INT_ID, testInt));
    ASSERT_TRUE(luna.readToVar(FLOAT_ID, testFloat));
    ASSERT_TRUE(luna.readToVar(BOOL_ID, testBool));
    ASSERT_TRUE(luna.readToVar(STR_ID, testString));

    ASSERT_EQ(testInt, INT_VALUE);
    ASSERT_EQ(testFloat, FLOAT_VALUE);
    ASSERT_EQ(testBool, BOOL_VALUE);
    ASSERT_EQ(testString, STR_VALUE);
}

TEST_F(LunaTest, IfVarNotDefinedInLua_LunaReadToVarWontChangeTargetVar)
{
    constexpr int       OLD_INT_VALUE{100};
    constexpr double    OLD_FLOAT_VALUE{231.20};
    constexpr bool      OLD_BOOL_VALUE{true};
    constexpr char      OLD_STR_VALUE[]{"TESTO"};
    int         testInt     = OLD_INT_VALUE;
    double      testFloat   = OLD_FLOAT_VALUE;
    bool        testBool    = OLD_BOOL_VALUE;
    std::string testString  = OLD_STR_VALUE;

    ASSERT_FALSE(luna.readToVar(INT_ID, testInt));
    ASSERT_FALSE(luna.readToVar(FLOAT_ID, testFloat));
    ASSERT_FALSE(luna.readToVar(BOOL_ID, testBool));
    ASSERT_FALSE(luna.readToVar(STR_ID, testString));

    ASSERT_EQ(testInt, OLD_INT_VALUE);
    ASSERT_EQ(testFloat, OLD_FLOAT_VALUE);
    ASSERT_EQ(testBool, OLD_BOOL_VALUE);
    ASSERT_EQ(testString, OLD_STR_VALUE);
}

TEST_F(LunaTest, LunaCanRunStringAsScript)
{
    constexpr char  SCRIPT_INT_ID[]{"SCRIPT_INT"};
    constexpr int   SCRIPT_INT_VALUE{120};
    constexpr char  SCRIPT_STR_ID[]{"SCRIPT_STR"};
    constexpr char  SCRIPT_STR_VALUE[]{"HELLO LUNA FROM STRING!"};

    std::stringstream script;
    script << SCRIPT_INT_ID << "=" << SCRIPT_INT_VALUE              << "\n";
    script << SCRIPT_STR_ID << "=" << std::quoted(SCRIPT_STR_VALUE) << "\n";

    luna.doString(script.str());

    auto readInt = luna.readInt(SCRIPT_INT_ID);
    auto readStr = luna.readString(SCRIPT_STR_ID);

    ASSERT_TRUE(readInt);
    ASSERT_TRUE(readStr);

    ASSERT_EQ(readInt, SCRIPT_INT_VALUE);
    ASSERT_EQ(readStr, SCRIPT_STR_VALUE);
}
