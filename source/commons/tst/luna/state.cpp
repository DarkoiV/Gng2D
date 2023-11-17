#include "Gng2D/commons/luna/state.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>
#include <sstream>

constexpr char   INT_ID[]    = "TEST_INT";
constexpr int    INT_VALUE   = 129;
constexpr char   FLOAT_ID[]  = "TEST_FLOAT";
constexpr double FLOAT_VALUE = 129.389;
constexpr char   BOOL_ID[]   = "TEST_BOOL";
constexpr bool   BOOL_VALUE  = false;
constexpr char   STR_ID[]    = "TEST_STRING";
constexpr char   STR_VALUE[] = "HELLO LUNA!";

struct LunaTest : ::testing::Test
{
    inline static std::string           testFilesDir{TEST_FILES_DIR};
    inline static std::string           testFileName{"test.lua"};
    inline static std::filesystem::path testFile{testFilesDir + "/" + testFileName};
    static void                         SetUpTestSuite()
    {
        std::fstream file(testFile, std::fstream::out);
        std::boolalpha(file);
        file << INT_ID << " = " << INT_VALUE << "\n";
        file << FLOAT_ID << " = " << FLOAT_VALUE << "\n";
        file << BOOL_ID << " = " << BOOL_VALUE << "\n";
        file << STR_ID << " = " << std::quoted(STR_VALUE) << std::endl;
    }

    Gng2D::Luna::State luna;
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

TEST_F(LunaTest, WhenReadingValueOfWrongType_LunaWillReturnNullopt)
{
    luna.doFile(testFile.string());
    ASSERT_EQ(luna.readInt(BOOL_ID), std::nullopt);
    ASSERT_EQ(luna.readFloat(STR_ID), std::nullopt);
    ASSERT_EQ(luna.readBool(STR_ID), std::nullopt);
    ASSERT_EQ(luna.readString(INT_ID), std::nullopt);
}

TEST_F(LunaTest, ReadFloatCanReadIntegerValues_OppositeHoweverIsNotTrue)
{
    luna.doFile(testFile.string());
    ASSERT_TRUE(luna.readFloat(INT_ID));
    ASSERT_FALSE(luna.readInt(FLOAT_ID));
    ASSERT_EQ(*luna.readFloat(INT_ID), static_cast<Gng2D::Luna::Integer>(INT_VALUE));
}

TEST_F(LunaTest, ReadGlobalCanBeUsedToReadVarOfUnknownType)
{
    luna.doFile(testFile);

    auto readInt    = luna.read(INT_ID);
    auto readFloat  = luna.read(FLOAT_ID);
    auto readString = luna.read(STR_ID);
    auto readBool   = luna.read(BOOL_ID);
    auto readNotDef = luna.read("NOT_DEF_VALUE");

    ASSERT_TRUE(std::holds_alternative<Gng2D::Luna::Integer>(readInt));
    ASSERT_TRUE(readInt.isInteger());
    ASSERT_TRUE(std::holds_alternative<Gng2D::Luna::Float>(readFloat));
    ASSERT_TRUE(readFloat.isFloat());
    ASSERT_TRUE(std::holds_alternative<Gng2D::Luna::String>(readString));
    ASSERT_TRUE(readString.isString());
    ASSERT_TRUE(std::holds_alternative<bool>(readBool));
    ASSERT_TRUE(readBool.isBool());
    ASSERT_TRUE(std::holds_alternative<Gng2D::Luna::Nil>(readNotDef));
    ASSERT_TRUE(readNotDef.isNil());

    ASSERT_EQ(std::get<Gng2D::Luna::Integer>(readInt), INT_VALUE);
    ASSERT_EQ(readInt.asInteger(), INT_VALUE);
    ASSERT_EQ(std::get<Gng2D::Luna::Float>(readFloat), FLOAT_VALUE);
    ASSERT_EQ(readFloat.asFloat(), FLOAT_VALUE);
    ASSERT_EQ(std::get<bool>(readBool), BOOL_VALUE);
    ASSERT_EQ(readBool.asBool(), BOOL_VALUE);
    ASSERT_EQ(std::get<Gng2D::Luna::String>(readString), STR_VALUE);
    ASSERT_EQ(readString.asString(), STR_VALUE);
}

// TODO move to to LunaType test
TEST_F(LunaTest, LunaTypeCanBeComparedAgainstUnderlyingTypes)
{
    luna.doFile(testFile.string());
    ASSERT_EQ(luna.read(INT_ID), INT_VALUE);
    ASSERT_EQ(luna.read(FLOAT_ID), FLOAT_VALUE);
    ASSERT_EQ(luna.read(STR_ID), STR_VALUE);
    ASSERT_EQ(luna.read(BOOL_ID), BOOL_VALUE);
    ASSERT_EQ(luna.read("NOT_DEF_VALUE"), Gng2D::Luna::Nil{});

    ASSERT_NE(luna.read(INT_ID), FLOAT_VALUE);
    ASSERT_NE(luna.read(FLOAT_ID), STR_VALUE);
    ASSERT_NE(luna.read(STR_ID), BOOL_VALUE);
    ASSERT_NE(luna.read(BOOL_ID), INT_VALUE);
    ASSERT_NE(luna.read("NOT_DEF_VALUE"), 33.0);

    ASSERT_NE(luna.read(INT_ID), Gng2D::Luna::Nil{});
    ASSERT_NE(luna.read(FLOAT_ID), Gng2D::Luna::Nil{});
    ASSERT_NE(luna.read(STR_ID), Gng2D::Luna::Nil{});
    ASSERT_NE(luna.read(BOOL_ID), Gng2D::Luna::Nil{});
}

TEST_F(LunaTest, LunaCanRunStringAsScript)
{
    constexpr char SCRIPT_INT_ID[]{"SCRIPT_INT"};
    constexpr int  SCRIPT_INT_VALUE{120};
    constexpr char SCRIPT_STR_ID[]{"SCRIPT_STR"};
    constexpr char SCRIPT_STR_VALUE[]{"HELLO LUNA FROM STRING!"};

    std::stringstream script;
    script << SCRIPT_INT_ID << "=" << SCRIPT_INT_VALUE << "\n";
    script << SCRIPT_STR_ID << "=" << std::quoted(SCRIPT_STR_VALUE) << "\n";

    luna.doString(script.str());

    auto readInt = luna.readInt(SCRIPT_INT_ID);
    auto readStr = luna.readString(SCRIPT_STR_ID);

    ASSERT_TRUE(readInt);
    ASSERT_TRUE(readStr);

    ASSERT_EQ(readInt, SCRIPT_INT_VALUE);
    ASSERT_EQ(readStr, SCRIPT_STR_VALUE);
}
