#include "Gng2D/commons/luna.hpp"
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

using Gng2D::Luna;

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
    ASSERT_EQ(*luna.readFloat(INT_ID), static_cast<Luna::Integer>(INT_VALUE));
}

TEST_F(LunaTest, ReadGlobalCanBeUsedToReadVarOfUnknownType)
{
    luna.doFile(testFile);

    auto readInt    = luna.read(INT_ID);
    auto readFloat  = luna.read(FLOAT_ID);
    auto readString = luna.read(STR_ID);
    auto readBool   = luna.read(BOOL_ID);
    auto readNotDef = luna.read("NOT_DEF_VALUE");

    ASSERT_TRUE(std::holds_alternative<Luna::Integer>(readInt));
    ASSERT_TRUE(readInt.isInteger());
    ASSERT_TRUE(std::holds_alternative<Luna::Float>(readFloat));
    ASSERT_TRUE(readFloat.isFloat());
    ASSERT_TRUE(std::holds_alternative<Luna::String>(readString));
    ASSERT_TRUE(readString.isString());
    ASSERT_TRUE(std::holds_alternative<bool>(readBool));
    ASSERT_TRUE(readBool.isBool());
    ASSERT_TRUE(std::holds_alternative<Luna::Nil>(readNotDef));
    ASSERT_TRUE(readNotDef.isNil());

    ASSERT_EQ(std::get<Luna::Integer>(readInt), INT_VALUE);
    ASSERT_EQ(readInt.asInteger(), INT_VALUE);
    ASSERT_EQ(std::get<Luna::Float>(readFloat), FLOAT_VALUE);
    ASSERT_EQ(readFloat.asFloat(), FLOAT_VALUE);
    ASSERT_EQ(std::get<bool>(readBool), BOOL_VALUE);
    ASSERT_EQ(readBool.asBool(), BOOL_VALUE);
    ASSERT_EQ(std::get<Luna::String>(readString), STR_VALUE);
    ASSERT_EQ(readString.asString(), STR_VALUE);
}

TEST_F(LunaTest, LunaTypeCanBeComparedAgainstUnderlyingTypes)
{
    luna.doFile(testFile.string());
    ASSERT_EQ(luna.read(INT_ID), INT_VALUE);
    ASSERT_EQ(luna.read(FLOAT_ID), FLOAT_VALUE);
    ASSERT_EQ(luna.read(STR_ID), STR_VALUE);
    ASSERT_EQ(luna.read(BOOL_ID), BOOL_VALUE);
    ASSERT_EQ(luna.read("NOT_DEF_VALUE"), Luna::Nil{});

    ASSERT_NE(luna.read(INT_ID), FLOAT_VALUE);
    ASSERT_NE(luna.read(FLOAT_ID), STR_VALUE);
    ASSERT_NE(luna.read(STR_ID), BOOL_VALUE);
    ASSERT_NE(luna.read(BOOL_ID), INT_VALUE);
    ASSERT_NE(luna.read("NOT_DEF_VALUE"), 33.0);

    ASSERT_NE(luna.read(INT_ID), Luna::Nil{});
    ASSERT_NE(luna.read(FLOAT_ID), Luna::Nil{});
    ASSERT_NE(luna.read(STR_ID), Luna::Nil{});
    ASSERT_NE(luna.read(BOOL_ID), Luna::Nil{});
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

TEST_F(LunaTest, LunaCanDoStackOpeartions)
{
    constexpr int   STACK_INT      = 12;
    constexpr float STACK_FLOAT    = 123.45;
    constexpr bool  STACK_BOOL     = false;
    constexpr char  STACK_STRING[] = "HELLO STACK!";

    Luna luna;
    luna.pushInt(STACK_INT);
    luna.pushFloat(STACK_FLOAT);
    luna.pushBool(STACK_BOOL);
    luna.pushString(STACK_STRING);

    auto readString = luna.readStack(-1);
    auto readBool   = luna.readStack(-2);
    auto readFloat  = luna.readStack(-3);
    auto readInt    = luna.readStack(-4);

    auto readString2 = luna.readStack(4);
    auto readBool2   = luna.readStack(3);
    auto readFloat2  = luna.readStack(2);
    auto readInt2    = luna.readStack(1);

    ASSERT_TRUE(std::holds_alternative<Luna::String>(readString));
    ASSERT_TRUE(std::holds_alternative<Luna::Bool>(readBool));
    ASSERT_TRUE(std::holds_alternative<Luna::Float>(readFloat));
    ASSERT_TRUE(std::holds_alternative<Luna::Integer>(readInt));

    ASSERT_TRUE(std::holds_alternative<Luna::String>(readString2));
    ASSERT_TRUE(std::holds_alternative<Luna::Bool>(readBool2));
    ASSERT_TRUE(std::holds_alternative<Luna::Float>(readFloat2));
    ASSERT_TRUE(std::holds_alternative<Luna::Integer>(readInt2));

    ASSERT_EQ(std::get<Luna::String>(readString), STACK_STRING);
    ASSERT_EQ(std::get<Luna::Bool>(readBool), STACK_BOOL);
    ASSERT_EQ(std::get<Luna::Float>(readFloat), STACK_FLOAT);
    ASSERT_EQ(std::get<Luna::Integer>(readInt), STACK_INT);

    ASSERT_EQ(std::get<Luna::String>(readString2), STACK_STRING);
    ASSERT_EQ(std::get<Luna::Bool>(readBool2), STACK_BOOL);
    ASSERT_EQ(std::get<Luna::Float>(readFloat2), STACK_FLOAT);
    ASSERT_EQ(std::get<Luna::Integer>(readInt2), STACK_INT);
}

TEST_F(LunaTest, TableBracketAcessToUndefinedVariables_ReturnsNil)
{
    Luna::Table table;
    table["FIRST_ACESS"];
    ASSERT_EQ(table["FIRST_ACESS"], Luna::Nil{});
}

constexpr static char TABLE_SCRIPT[] =
    "my_table = {}                          \n"
    "my_table[123]          = false         \n"
    "my_table[12.07]        = \"float!\"    \n"
    "my_table[\"test\"]     = \"testo\"     \n"
    "my_table[\"table\"]    = {}            \n"
    "my_table.table[\"float\"]  = 123.0     \n"
    "my_table.table[\"int\"]    = 2         \n";

TEST_F(LunaTest, LunaCanReadTableToCppMap)
{
    luna.doString(TABLE_SCRIPT);
    auto tableptr = luna.read("my_table");

    ASSERT_TRUE(Luna::is<Luna::Table>(tableptr));
    auto& table = std::get<Luna::Table>(tableptr);

    ASSERT_EQ(table[123], false);
    ASSERT_EQ(table["test"], "testo");
    ASSERT_EQ(table[12.07], "float!");

    ASSERT_TRUE(Luna::is<Luna::Table>(table["table"]));
    auto& innerTable = std::get<Luna::Table>(table["table"]);

    ASSERT_EQ(innerTable["float"], 123.0);
    ASSERT_EQ(innerTable["int"], 2ll);
}

const Luna::Table INNER_TEST_TABLE = {
    {"INNER_TABLE_FLOAT", Luna::Float{205.0}},
    {  Luna::Float(3.23), Luna::Float{33.33}},
};

const Luna::Table TEST_TABLE = {
    {  "TABLE_NIL",                      Luna::Nil{}},
    {  "TABLE_INT",               Luna::Integer{123}},
    {  "TABLE_STR", Luna::String{"Hello from table"}},
    {"TABLE_TABLE",                 INNER_TEST_TABLE},
    {          900,                "THIS IS INTEGER"},
};

TEST_F(LunaTest, LunaCanCreateTables)
{
    luna.pushTable(TEST_TABLE);

    auto stackObj = luna.readStack(-1);
    ASSERT_TRUE(Luna::is<Luna::Table>(stackObj));

    auto& table = std::get<Luna::Table>(stackObj);
    ASSERT_EQ(table["TABLE_NIL"], Luna::Nil{});
    ASSERT_EQ(table["TABLE_INT"], Luna::Integer{123});
    ASSERT_EQ(table["TABLE_STR"], "Hello from table");
    ASSERT_EQ(table[900], "THIS IS INTEGER");
    ASSERT_TRUE(Luna::is<Luna::Table>(table["TABLE_TABLE"]));

    auto& innerTable = std::get<Luna::Table>(table["TABLE_TABLE"]);
    ASSERT_EQ(innerTable["INNER_TABLE_FLOAT"], Luna::Float{205.0});
    ASSERT_EQ(innerTable[3.23], Luna::Float{33.33});
}
