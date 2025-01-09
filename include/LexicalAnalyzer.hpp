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

extern std::map<std::string, unsigned> nameTable;
extern std::ifstream file;
extern std::vector<std::vector<unsigned>> vectors;
extern std::vector<unsigned> currentVector;

// список кодов лексем
enum class LexemeCodes {
    // Операции сравнения
    EQUAL = 1,           // Равно
    NOT_EQUAL = 2,       // Не равно
    LESS = 3,           // Меньше
    GREATER = 4,        // Больше
    LESS_EQUAL = 5,     // Меньше либо равно
    GREATER_EQUAL = 6,  // Больше либо равно

    // Базовые операции
    PUSH = 995,
    POP = 996,
    ARITHMETIC_OPERATION = 997,
    RELATION = 998,
    JMP = 999,
    JI = 1000,
    READ = 1001,
    WRITE = 1002,
    END = 1003,
    COMMENT = 1004,
    ERROR = 1005,
    END_MARKER = 1006,

    // Арифметические операции
    ADD = 1007,
    SUB = 1008,
    MUL = 1009,
    DIV = 1010,
    MOD = 1011,

    // Переменные и константы
    VARIABLE = 1018,
    CONSTANT = 1019,

    // Векторные операции и разделители
    VECTOR_START = 1020,
    COMMA = 1021,
    VECTOR_END = 1022,
    VADD = 1023,
    VSUB = 1024,
    VMUL = 1025,
    VDIV = 1026,
    VMOD = 1027,
    VDOT = 1028,
    VCONCAT = 1029,
    VLSHIFT = 1030,
    VRSHIFT = 1031
};

// список лексем
enum class LexemeClass {
    PUSH = static_cast<int>(LexemeCodes::PUSH),
    POP = static_cast<int>(LexemeCodes::POP),
    ARITHMETIC_OPERATION = static_cast<int>(LexemeCodes::ARITHMETIC_OPERATION),
    RELATION = static_cast<int>(LexemeCodes::RELATION),
    JMP = static_cast<int>(LexemeCodes::JMP),
    JI = static_cast<int>(LexemeCodes::JI),
    READ = static_cast<int>(LexemeCodes::READ),
    WRITE = static_cast<int>(LexemeCodes::WRITE),
    END = static_cast<int>(LexemeCodes::END),
    COMMENT = static_cast<int>(LexemeCodes::COMMENT),
    ERROR = static_cast<int>(LexemeCodes::ERROR),
    END_MARKER = static_cast<int>(LexemeCodes::END_MARKER),
    ADD = static_cast<int>(LexemeCodes::ADD),
    SUB = static_cast<int>(LexemeCodes::SUB),
    MUL = static_cast<int>(LexemeCodes::MUL),
    DIV = static_cast<int>(LexemeCodes::DIV),
    MOD = static_cast<int>(LexemeCodes::MOD),
    LESS = static_cast<int>(LexemeCodes::LESS),
    GREATER = static_cast<int>(LexemeCodes::GREATER),
    LESS_EQUAL = static_cast<int>(LexemeCodes::LESS_EQUAL),
    GREATER_EQUAL = static_cast<int>(LexemeCodes::GREATER_EQUAL),
    EQUAL = static_cast<int>(LexemeCodes::EQUAL),
    NOT_EQUAL = static_cast<int>(LexemeCodes::NOT_EQUAL),
    VARIABLE = static_cast<int>(LexemeCodes::VARIABLE),
    CONSTANT = static_cast<int>(LexemeCodes::CONSTANT),
    VECTOR_START = static_cast<int>(LexemeCodes::VECTOR_START),
    COMMA = static_cast<int>(LexemeCodes::COMMA),
    VECTOR_END = static_cast<int>(LexemeCodes::VECTOR_END),
    VADD = static_cast<int>(LexemeCodes::VADD),
    VSUB = static_cast<int>(LexemeCodes::VSUB),
    VMUL = static_cast<int>(LexemeCodes::VMUL),
    VDIV = static_cast<int>(LexemeCodes::VDIV),
    VMOD = static_cast<int>(LexemeCodes::VMOD),
    VDOT = static_cast<int>(LexemeCodes::VDOT),
    VCONCAT = static_cast<int>(LexemeCodes::VCONCAT),
    VLSHIFT = static_cast<int>(LexemeCodes::VLSHIFT),
    VRSHIFT = static_cast<int>(LexemeCodes::VRSHIFT),
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
    {static_cast<unsigned>(LexemeCodes::NOT_EQUAL), 0, 0, 0},
    {static_cast<unsigned>(LexemeCodes::LESS_EQUAL), 0, 0, 0},
    {static_cast<unsigned>(LexemeCodes::GREATER_EQUAL), 0, 0, 0}
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

constexpr static unsigned short numberStates = 27; // Количество состояний
constexpr static unsigned short numberClass = 12; // Количество символьных лексем

using functionPointer = States (*)();
using TransitionTable = std::array<std::array<functionPointer, numberClass>, numberStates>;

// Вектор альтернатив
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

// Начальный вектор
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