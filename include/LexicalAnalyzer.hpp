#ifndef LEXICALANALYZER_HPP
#define LEXICALANALYZER_HPP

#include <array>
#include <vector>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <unordered_map>
#include <variant>

static std::map<std::string, unsigned> nameTable;
static std::ifstream file;
static std::vector<std::vector<unsigned>> vectors;
static std::vector<unsigned> currentVector;

// значения лексем
static constexpr short EQUAL_CODE = 1; // равно
static constexpr short NOT_EQUAL_CODE = 2; // не равно
static constexpr short LESS_CODE = 3; // меньше
static constexpr short GREATER_CODE = 4; // больше
static constexpr short LESS_EQUAL_CODE = 5; // меньше либо равно
static constexpr short GREATER_EQUAL_CODE = 6; // больше либо равно
static constexpr short PUSH_CODE = 995;
static constexpr short POP_CODE = 996;
static constexpr short ARITHMETIC_OPERATION_CODE = 997;
static constexpr short RELATION_CODE = 998;
static constexpr short JMP_CODE = 999;
static constexpr short JI_CODE = 1000;
static constexpr short READ_CODE = 1001;
static constexpr short WRITE_CODE = 1002;
static constexpr short END_CODE = 1003;
static constexpr short COMMENT_CODE = 1004;
static constexpr short ERROR_CODE = 1005;
static constexpr short END_MARKER_CODE = 1006;
static constexpr short ADD_CODE = 1007;
static constexpr short SUB_CODE = 1008;
static constexpr short MUL_CODE = 1009;
static constexpr short DIV_CODE = 1010;
static constexpr short MOD_CODE = 1011;
static constexpr short VARIABLE_CODE = 1018;
static constexpr short CONSTANT_CODE = 1019;
static constexpr short VECTOR_START_CODE = 1020;
static constexpr short COMMA_CODE = 1021;
static constexpr short VECTOR_END_CODE = 1022;
static constexpr short VADD_CODE = 1023;
static constexpr short VSUB_CODE = 1024;
static constexpr short VMUL_CODE = 1025;
static constexpr short VDIV_CODE = 1026;
static constexpr short VMOD_CODE = 1027;
static constexpr short VDOT_CODE = 1028;
static constexpr short VCONCAT_CODE = 1029;
static constexpr short VLSHIFT_CODE = 1030;
static constexpr short VRSHIFT_CODE = 1031;

// список лексем
enum class LexemeClass {
    PUSH = PUSH_CODE,
    POP = POP_CODE,
    ARITHMETIC_OPERATION = ARITHMETIC_OPERATION_CODE,
    RELATION = RELATION_CODE,
    JMP = JMP_CODE,
    JI = JI_CODE,
    READ = READ_CODE,
    WRITE = WRITE_CODE,
    END = END_CODE,
    COMMENT = COMMENT_CODE,
    ERROR = ERROR_CODE,
    END_MARKER = END_MARKER_CODE,
    ADD = ADD_CODE,
    SUB = SUB_CODE,
    MUL = MUL_CODE,
    DIV = DIV_CODE,
    MOD = MOD_CODE,
    LESS = LESS_CODE,
    GREATER = GREATER_CODE,
    LESS_EQUAL = LESS_EQUAL_CODE,
    GREATER_EQUAL = GREATER_EQUAL_CODE,
    EQUAL = EQUAL_CODE,
    NOT_EQUAL = NOT_EQUAL_CODE,
    VARIABLE = VARIABLE_CODE,
    CONSTANT = CONSTANT_CODE,
    VECTOR_START = VECTOR_START_CODE,
    COMMA = COMMA_CODE,
    VECTOR_END = VECTOR_END_CODE,
    VADD = VADD_CODE,
    VSUB = VSUB_CODE,
    VMUL = VMUL_CODE,
    VDIV = VDIV_CODE,
    VMOD = VMOD_CODE,
    VDOT = VDOT_CODE,
    VCONCAT = VCONCAT_CODE,
    VLSHIFT = VLSHIFT_CODE,
    VRSHIFT = VRSHIFT_CODE,
};

// список символьных лексем
enum class SymbolicTokenClass {
    LETTER, DIGIT, ARITHMETIC_OPERATION, COMPARISON_OPERATION,
    SPACE_OR_TAB, END_OF_LINE, SEMICOLON, VECTOR_SYMBOL, COMMA, ERROR, END_OF_FILE
};

// состояния
enum class States {
    states_A1, states_A2, states_B1, states_C1, states_D1, states_E1, states_E2, states_E3, states_F1, states_F2,
    states_F3, states_G1, states_H1, states_I1, states_I2, states_J1, states_M1, states_STOP, states_V1, states_V2
};

// структура для представления символьной лексемы
struct SymbolicToken {
    SymbolicTokenClass tokenClass;
    unsigned value;
};

// структура для представления лексемы
struct Lexeme {
    LexemeClass lexemeClass;
    unsigned value;
    unsigned lineNumber;
};

inline std::vector<Lexeme> lexemes;
inline std::set<unsigned> constantTable;

inline unsigned numberRegister = 0; // регистр числа
inline unsigned short classRegister; // хранит класс лексемы
inline char relationRegister = '\0'; // регистр отношения
inline std::string variableRegister; // регистр имени переменной
inline unsigned lineNumber = 1; // текущий номер строки программы
inline bool constantFlag = false; // флаг, указывающий на константу
inline short detectionRegister; // регистр обнаружения
inline unsigned pointerRegister = 0; // регистр указателя

inline std::unordered_map<LexemeClass, std::string> operationMap = {
    {LexemeClass::ADD, "+"},
    {LexemeClass::SUB, "-"},
    {LexemeClass::MUL, "*"},
    {LexemeClass::DIV, "/"},
    {LexemeClass::MOD, "%"},
    {LexemeClass::LESS, "<"},
    {LexemeClass::GREATER, ">"},
    {LexemeClass::LESS_EQUAL, "<="},
    {LexemeClass::GREATER_EQUAL, ">="},
    {LexemeClass::EQUAL, "=="},
    {LexemeClass::NOT_EQUAL, "!="}
};

void addNameToTable(const std::string &name);

void addConstant(unsigned short &pointerRegister, unsigned numberRegister, bool constantFlag);

extern void createLexeme(LexemeClass classRegister, unsigned pointerRegister, unsigned numberRegister,
                         unsigned relationRegister,
                         unsigned lineNumber);

SymbolicToken transliterator(int ch);

inline unsigned relationTable[4][4] = {
    {0, 0, 0, 0},
    {NOT_EQUAL_CODE, 0, 0, 0},
    {LESS_EQUAL_CODE, 0, 0, 0},
    {GREATER_EQUAL_CODE, 0, 0, 0}
};

short processRelation(char first, char second);

inline SymbolicToken globalSymbol;
inline std::string globalComment;

using Procedure = States(*)();
using VariantType = std::variant<unsigned long, SymbolicTokenClass>;
extern std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure>> vectorOfAlternatives;
extern std::map<char, VariantType> initialMap;
inline std::vector<std::string> variables;

void addVariable();

std::string getLexemeValueString(LexemeClass lexemeClass, unsigned value);

States A1();

States A1a();

States A1b();

States A2();

States A2a();

States A2b();

States A2c();

States A2d();

States A2e();

States A2f();

States B1a();

States B1b();

States C1();

States C1a();

States C1b();

States C1c();

States C1d();

States C1e();

States C1f();

States C1g();

States C1h();

States D1a();

States E1a();

States E2a();

States E2b();

States E3a();

States F1();

States F2();

States F3();

States G1a();

States G1b();

States H1a();

States H1b();

States I1();

States I1a();

States I2();

States I2a();

States I2b();

States I2c();

States I2d();

States J1();

States M1();

States V1();

States V2();

States handleVCommand();

States handleVectorStart();

States handleVSubCommand();

States handleVMulCommand();

States handleVDivCommand();

States handleVModCommand();

States handleVDotCommand();

States handleVConcatCommand();

States handleVLShiftCommand();

States handleVRShiftCommand();

States EXIT1();

States EXIT2();

States EXIT3();

States EXIT4();

States ERROR1(const unsigned &lineNumber);

constexpr unsigned short numberStates = 27; // Количество состояний
constexpr unsigned short numberClass = 12; // Количество символьных лексем

using functionPointer = States (*)();
using TransitionTable = std::array<std::array<functionPointer, numberClass>, numberStates>;

inline std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure>> vectorOfAlternatives = {
    {1, 'n', std::nullopt, B1b},
    {2, 'd', std::nullopt, C1b},

    {3, 'i', std::make_optional(4UL), E2a},

    {4, 'm', std::nullopt, B1b},
    {5, 'p', std::nullopt, E2b},

    {6, 'o', std::make_optional(8UL), B1b},
    {7, 'p', std::nullopt, E3a},

    {8, 'u', std::nullopt, B1b},
    {9, 's', std::nullopt, B1b},
    {10, 'h', std::nullopt, E1a},

    {11, 'e', std::nullopt, B1b},
    {12, 'a', std::nullopt, B1b},
    {13, 'd', std::nullopt, C1c},

    {14, 'r', std::nullopt, B1b},
    {15, 'i', std::nullopt, B1b},
    {16, 't', std::nullopt, B1b},
    {17, 'e', std::nullopt, C1d},

    {18, 'a', std::make_optional(21UL), B1b},
    {19, 'd', std::nullopt, B1b},
    {20, 'd', std::nullopt, handleVCommand},

    {21, 's', std::make_optional(24UL), B1b},
    {22, 'u', std::nullopt, B1b},
    {23, 'b', std::nullopt, handleVSubCommand},

    {24, 'm', std::make_optional(27UL), B1b},
    {25, 'u', std::make_optional(31UL), B1b},
    {26, 'l', std::nullopt, handleVMulCommand},

    {27, 'd', std::make_optional(30UL), B1b},
    {28, 'i', std::nullopt, B1b},
    {29, 'v', std::nullopt, handleVDivCommand},

    {30, 'm', std::make_optional(33UL), B1b},
    {31, 'o', std::nullopt, B1b},
    {32, 'd', std::nullopt, handleVModCommand},

    {33, 'd', std::make_optional(36UL), B1b},
    {34, 'o', std::nullopt, B1b},
    {35, 't', std::nullopt, handleVDotCommand},

    {36, 'c', std::make_optional(42UL), B1b},
    {37, 'o', std::nullopt, B1b},
    {38, 'n', std::nullopt, B1b},
    {39, 'c', std::nullopt, B1b},
    {40, 'a', std::nullopt, B1b},
    {41, 't', std::nullopt, handleVConcatCommand},

    {42, 'l', std::make_optional(48UL), B1b},
    {43, 's', std::nullopt, B1b},
    {44, 'h', std::nullopt, B1b},
    {45, 'i', std::nullopt, B1b},
    {46, 'f', std::nullopt, B1b},
    {47, 't', std::nullopt, handleVLShiftCommand},

    {48, 'r', std::nullopt, B1b},
    {49, 's', std::nullopt, B1b},
    {50, 'h', std::nullopt, B1b},
    {51, 'i', std::nullopt, B1b},
    {52, 'f', std::nullopt, B1b},
    {53, 't', std::nullopt, handleVRShiftCommand}
};

inline std::map<char, VariantType> initialMap{
    {'a', VariantType{SymbolicTokenClass::ERROR}},
    {'b', VariantType{SymbolicTokenClass::ERROR}},
    {'c', VariantType{SymbolicTokenClass::ERROR}},
    {'d', VariantType{SymbolicTokenClass::ERROR}},
    {'e', VariantType{static_cast<unsigned long>(1)}},
    {'f', VariantType{SymbolicTokenClass::ERROR}},
    {'g', VariantType{SymbolicTokenClass::ERROR}},
    {'h', VariantType{SymbolicTokenClass::ERROR}},
    {'i', VariantType{SymbolicTokenClass::ERROR}},
    {'j', VariantType{static_cast<unsigned long>(3)}},
    {'k', VariantType{SymbolicTokenClass::ERROR}},
    {'l', VariantType{SymbolicTokenClass::ERROR}},
    {'m', VariantType{SymbolicTokenClass::ERROR}},
    {'n', VariantType{SymbolicTokenClass::ERROR}},
    {'o', VariantType{SymbolicTokenClass::ERROR}},
    {'p', VariantType{static_cast<unsigned long>(6)}},
    {'q', VariantType{SymbolicTokenClass::ERROR}},
    {'r', VariantType{static_cast<unsigned long>(11)}},
    {'s', VariantType{SymbolicTokenClass::ERROR}},
    {'t', VariantType{SymbolicTokenClass::ERROR}},
    {'u', VariantType{SymbolicTokenClass::ERROR}},
    {'v', VariantType{static_cast<unsigned long>(18)}},
    {'w', VariantType{static_cast<unsigned long>(14)}},
    {'x', VariantType{SymbolicTokenClass::ERROR}},
    {'y', VariantType{SymbolicTokenClass::ERROR}},
    {'z', VariantType{SymbolicTokenClass::ERROR}}
};

TransitionTable initializeTable();

void parse(const std::string &filePath);

#endif //LEXICALANALYZER_HPP
