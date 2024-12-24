#ifndef LEXICALANALYZER_HPP
#define LEXICALANALYZER_HPP
#include <algorithm>
#include <array>
#include <vector>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <unordered_map>
#include <variant>

static std::map<std::string, unsigned> nameTable;
static unsigned nextVariableID = 1;
static std::ifstream file;
static std::vector<std::vector<unsigned>> vectors;
static bool isParsingVector = false;
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
    LETTER, DIGIT, SIGN, ARITHMETIC_OPERATION, COMPARISON_OPERATION,
    SPACE_OR_TAB, END_OF_LINE, SEMICOLON, VECTOR_SYMBOL, COMMA, ERROR, END_OF_FILE
};

// состояния
enum class States {
    states_A1, states_A2, states_B1, states_C1, states_D1, states_E1, states_E2, states_E3, states_F1, states_F2,
    states_F3, states_G1, states_H1, states_I1, states_I2, states_J1, states_M1, states_STOP, EXIT1, EXIT2, EXIT3,
    EXIT4, ERROR1,  states_V1, states_V2, states_V3, handleVCommand
};

// структура для представления символьной лексемы
struct SymbolicToken {
    SymbolicTokenClass tokenClass;
    unsigned value;
};

SymbolicToken symbol{}; // символьная лексема, используется автоматом

// структура для представления лексемы
struct Lexeme {
    LexemeClass lexemeClass;
    unsigned value;
    unsigned lineNumber;
};

std::vector<Lexeme> lexemes;

struct ConstantEntry {
    unsigned value;

    explicit ConstantEntry(unsigned val) : value(val) {
    }
};

std::set<unsigned> constantTable;

unsigned numberRegister = 0; // регистр числа
unsigned short classRegister; // хранит класс лексемы
short signRegister = 1; // регистр знака
char relationRegister = '\0'; // регистр отношения
LexemeClass lexemeClass; // класс текущей лексемы
std::string variableRegister; // регистр имени переменной
unsigned valueRegister = 0; // регистр числового значения
unsigned lineNumber = 1; // текущий номер строки программы
bool constantFlag = false; // флаг, указывающий на константу
short detectionRegister; // регистр обнаружения
unsigned pointerRegister = 0; // регистр указателя

std::unordered_map<LexemeClass, std::string> operationMap = {
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

unsigned getVariableID(const std::string &variableName) {
    auto it = nameTable.find(variableName);
    if (it != nameTable.end())
        return it->second;

    nameTable[variableName] = nextVariableID++;
    return nameTable[variableName];
}

std::vector<std::string> convertLexemesToProgram(const std::vector<Lexeme> &lexemes) {
    std::vector<std::string> program;

    for (const auto &lexeme: lexemes) {
        std::string line;

        switch (lexeme.lexemeClass) {
            case LexemeClass::READ:
                line = "read";
                break;

            case LexemeClass::POP:
                if (lexeme.value < nameTable.size()) {
                    for (const auto &entry: nameTable) {
                        if (entry.second == lexeme.value) {
                            line = "pop " + entry.first; // Добавляем имя переменной
                            break;
                        }
                    }
                }
                break;

            case LexemeClass::PUSH:
                if (lexeme.value < nameTable.size()) {
                    for (const auto &entry: nameTable) {
                        if (entry.second == lexeme.value) {
                            line = "push " + entry.first;
                            break;
                        }
                    }
                } else
                    line = "push " + std::to_string(lexeme.value);
                break;

            case LexemeClass::VARIABLE:
                for (const auto &entry: nameTable) {
                    if (entry.second == lexeme.value) {
                        line = entry.first;
                        break;
                    }
                }
                break;

            case LexemeClass::CONSTANT:
                line = std::to_string(lexeme.value);
                break;

            case LexemeClass::END:
                line = "end";
                break;

            case LexemeClass::WRITE:
                line = "write";
                break;

            case LexemeClass::JMP:
                line = "jmp " + std::to_string(lexeme.value);
                break;

            case LexemeClass::JI:
                line = "ji " + std::to_string(lexeme.value);
                break;

            case LexemeClass::ARITHMETIC_OPERATION:
                switch (lexeme.value) {
                    case ADD_CODE: line = "add";
                        break;
                    case SUB_CODE: line = "sub";
                        break;
                    case MUL_CODE: line = "mul";
                        break;
                    case DIV_CODE: line = "div";
                        break;
                    case MOD_CODE: line = "mod";
                        break;
                    default: line = "unknown operation";
                        break;
                }
                break;

            case LexemeClass::RELATION:
                switch (lexeme.value) {
                    case static_cast<unsigned>(LexemeClass::GREATER): line = ">";
                        break;
                    case static_cast<unsigned>(LexemeClass::LESS): line = "<";
                        break;
                    case static_cast<unsigned>(LexemeClass::EQUAL): line = "==";
                        break;

                    default: line = "unknown relation";
                        break;
                }
                break;

            default:
                line = "unknown lexeme";
                break;
        }

        if (!line.empty())
            program.push_back(line);
    }

    return program;
}

void addNameToTable(const std::string &name) {
    auto it = nameTable.find(name);

    if (it != nameTable.end())
        pointerRegister = it->second;
    else {
        auto newIndex = static_cast<unsigned>(nameTable.size());

        nameTable[name] = newIndex;

        pointerRegister = newIndex;
    }
}

void addConstant(unsigned short &pointerRegister, unsigned numberRegister, bool constantFlag) {
    if (constantFlag == 0)
        return;

    constantTable.insert(numberRegister);
    pointerRegister = static_cast<unsigned short>(constantTable.size() - 1);
}

extern void createLexeme(LexemeClass classRegister, unsigned pointerRegister, unsigned numberRegister,
                         unsigned relationRegister,
                         unsigned lineNumber);

// SymbolicToken transliterator(int ch) {
//     SymbolicToken symbol{};
//     symbol.value = 0;
//
//     if (isalpha(ch)) {
//         symbol.tokenClass = SymbolicTokenClass::LETTER;
//         symbol.value = static_cast<unsigned>(std::tolower(ch));
//     } else if (isdigit(ch)) {
//         symbol.tokenClass = SymbolicTokenClass::DIGIT;
//         symbol.value = static_cast<unsigned>(ch - '0');
//     } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') {
//         symbol.tokenClass = SymbolicTokenClass::ARITHMETIC_OPERATION;
//         symbol.value = static_cast<unsigned>(ch);
//     } else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
//         if (ch == '<' && file.peek() == '<') {
//             file.get();
//             symbol.tokenClass = SymbolicTokenClass::VECTOR_SYMBOL;
//             symbol.value = VECTOR_START_CODE;
//         } else if (ch == '>' && file.peek() == '>') {
//             file.get();
//             symbol.tokenClass = SymbolicTokenClass::VECTOR_SYMBOL;
//             symbol.value = VECTOR_END_CODE;
//         } else {
//             symbol.tokenClass = SymbolicTokenClass::COMPARISON_OPERATION;
//             symbol.value = static_cast<unsigned>(ch);
//         }
//     } else if (ch == ',') {
//         symbol.tokenClass = SymbolicTokenClass::COMMA;
//         symbol.value = static_cast<unsigned>(ch);
//     } else if (ch == ' ' || ch == '\t')
//         symbol.tokenClass = SymbolicTokenClass::SPACE_OR_TAB;
//     else if (ch == '\n')
//         symbol.tokenClass = SymbolicTokenClass::END_OF_LINE;
//     else if (ch == EOF)
//         symbol.tokenClass = SymbolicTokenClass::END_OF_FILE;
//     else if (ch == ';') {
//         symbol.tokenClass = SymbolicTokenClass::SEMICOLON;
//         symbol.value = static_cast<unsigned>(ch);
//     } else
//         symbol.tokenClass = SymbolicTokenClass::ERROR;
//
//     return symbol;
// }

SymbolicToken transliterator(int ch) {
    static bool inComment = false;

    SymbolicToken symbol{};
    symbol.value = 0;

    if (ch == ';') {
        inComment = true;
        symbol.tokenClass = SymbolicTokenClass::SEMICOLON;
        symbol.value = static_cast<unsigned>(ch);
        return symbol;
    }

    if (ch == '\n') {
        inComment = false;
        symbol.tokenClass = SymbolicTokenClass::END_OF_LINE;
        return symbol;
    }

    if (inComment) {
        symbol.tokenClass = SymbolicTokenClass::SEMICOLON;
        symbol.value = static_cast<unsigned>(ch);
        return symbol;
    }

    if (isalpha(ch)) {
        symbol.tokenClass = SymbolicTokenClass::LETTER;
        symbol.value = static_cast<unsigned>(std::tolower(ch));
    } else if (isdigit(ch)) {
        symbol.tokenClass = SymbolicTokenClass::DIGIT;
        symbol.value = static_cast<unsigned>(ch - '0');
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') {
        symbol.tokenClass = SymbolicTokenClass::ARITHMETIC_OPERATION;
        symbol.value = static_cast<unsigned>(ch);
    } else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
        if (ch == '<' && file.peek() == '<') {
            file.get();
            symbol.tokenClass = SymbolicTokenClass::VECTOR_SYMBOL;
            symbol.value = VECTOR_START_CODE;
        } else if (ch == '>' && file.peek() == '>') {
            file.get();
            symbol.tokenClass = SymbolicTokenClass::VECTOR_SYMBOL;
            symbol.value = VECTOR_END_CODE;
        } else {
            symbol.tokenClass = SymbolicTokenClass::COMPARISON_OPERATION;
            symbol.value = static_cast<unsigned>(ch);
        }
    } else if (ch == ',') {
        symbol.tokenClass = SymbolicTokenClass::COMMA;
        symbol.value = static_cast<unsigned>(ch);
    } else if (ch == ' ' || ch == '\t') {
        symbol.tokenClass = SymbolicTokenClass::SPACE_OR_TAB;
    } else if (ch == EOF) {
        symbol.tokenClass = SymbolicTokenClass::END_OF_FILE;
    } else {
        symbol.tokenClass = SymbolicTokenClass::ERROR;
    }

    return symbol;
}

unsigned relationTable[4][4] = {
    {0, 0, 0, 0},
    {NOT_EQUAL_CODE, 0, 0, 0},
    {LESS_EQUAL_CODE, 0, 0, 0},
    {GREATER_EQUAL_CODE, 0, 0, 0}
};

short processRelation(char first, char second) {
    short row = -1;
    short col = -1;

    if (first == '=')
        row = 0;
    else if (first == '!')
        row = 1;
    else if (first == '<')
        row = 2;
    else if (first == '>')
        row = 3;

    if (second == '=')
        col = 0;
    else if (second == '!')
        col = 1;
    else if (second == '<')
        col = 2;
    else if (second == '>')
        col = 3;

    if (row != -1 && col != -1)
        return static_cast<short>(relationTable[row][col]);

    return -1;
}

SymbolicToken globalSymbol;
std::string globalComment;

using Procedure = States(*)();
using VariantType = std::variant<unsigned long, SymbolicTokenClass>;
extern std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure> > vectorOfAlternatives;
extern std::map<char, VariantType> initialMap;
std::vector<std::string> variables;

void addVariable() {
    if (variableRegister.find('<') != std::string::npos ||
        variableRegister.find('>') != std::string::npos ||
        variableRegister.find(',') != std::string::npos) {
        return;
    }

    static const std::array<std::string, 16> keyWords = {"push", "pop", "jmp", "ji", "read", "write", "end", "vadd", "vsub", "vmul", "vdiv", "vmod", "vdot", "vconcat", "vlshift", "vrshift"};
    for (const auto &keyWord: keyWords)
        if (variableRegister == keyWord) {
            std::cerr << "Имя переменной совпадает с одним из ключевых слов" << std::endl;
            return;
        }

    addNameToTable(variableRegister);
}

void createLexeme(LexemeClass classRegister, unsigned pointerRegister, unsigned numberRegister,
                  unsigned relationRegister, unsigned lineNumber) {
    if (classRegister == LexemeClass::COMMENT)
        return;

    Lexeme newLexeme{};
    newLexeme.lexemeClass = classRegister;
    newLexeme.lineNumber = lineNumber;

    switch (classRegister) {
        case LexemeClass::ARITHMETIC_OPERATION:
        case LexemeClass::RELATION:
            newLexeme.value = relationRegister;
            break;
        case LexemeClass::CONSTANT:
            newLexeme.value = numberRegister;
            break;
        case LexemeClass::VARIABLE:
            if (!variableRegister.empty())
                addNameToTable(variableRegister);
            newLexeme.value = pointerRegister;
            break;
        case LexemeClass::VADD:
        case LexemeClass::VSUB:
        case LexemeClass::VMUL:
        case LexemeClass::VDIV:
        case LexemeClass::VMOD:
        case LexemeClass::VDOT:
        case LexemeClass::VCONCAT:
        case LexemeClass::VLSHIFT:
        case LexemeClass::VRSHIFT:
            newLexeme.value = static_cast<unsigned>(classRegister);
            break;
        default:
            newLexeme.value = pointerRegister;
            break;
    }

    lexemes.push_back(newLexeme);
}

std::string getLexemeValueString(LexemeClass lexemeClass, unsigned value) {
    switch (lexemeClass) {
        case LexemeClass::PUSH: return "PUSH";
        case LexemeClass::POP: return "POP";
        case LexemeClass::ARITHMETIC_OPERATION: return std::string(1, static_cast<char>(value));
        case LexemeClass::RELATION: return std::string(1, static_cast<char>(value));
        case LexemeClass::GREATER: return ">";
        case LexemeClass::LESS: return "<";
        case LexemeClass::EQUAL: return "=";
        case LexemeClass::NOT_EQUAL: return "!=";
        case LexemeClass::GREATER_EQUAL: return ">=";
        case LexemeClass::LESS_EQUAL: return "<=";
        case LexemeClass::VARIABLE:
            for (const auto &[name, index]: nameTable)
                if (index == value)
                    return name;
            return std::to_string(value);
        case LexemeClass::CONSTANT: return std::to_string(value);
        case LexemeClass::JMP: return "JMP";
        case LexemeClass::JI: return "JI";
        case LexemeClass::READ: return "READ";
        case LexemeClass::WRITE: return "WRITE";
        case LexemeClass::END: return "END";
        case LexemeClass::COMMENT: return "COMMENT";
        case LexemeClass::ERROR: return "ERROR";
        case LexemeClass::END_MARKER: return "END_MARKER";
        case LexemeClass::MUL: return "*";
        case LexemeClass::ADD: return "+";
        case LexemeClass::SUB: return "-";
        case LexemeClass::DIV: return "/";
        case LexemeClass::MOD: return "%";
        case LexemeClass::VECTOR_START: return "<<";
        case LexemeClass::COMMA: return ",";
        case LexemeClass::VECTOR_END: return ">>";
        case LexemeClass::VADD: return "VADD";
        case LexemeClass::VSUB: return "VSUB";
        case LexemeClass::VMUL: return "VMUL";
        case LexemeClass::VDIV: return "VDIV";
        case LexemeClass::VMOD: return "VMOD";
        case LexemeClass::VDOT: return "VDOT";
        case LexemeClass::VCONCAT: return "VCONCAT";
        case LexemeClass::VLSHIFT: return "VLSHIFT";
        case LexemeClass::VRSHIFT: return "VRSHIFT";
        default: return "UNKNOWN";
    }
}

extern States ERROR1(const unsigned &lineNumber);

States A1() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB)
        return States::states_A1;
    else
        return ERROR1(lineNumber);
}

States A1a() {
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A1;
}

States A1b() {
    lineNumber++;

    return States::states_A1;
}

States A2() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB) {
        return States::states_A2;
    } else
        return ERROR1(lineNumber);
}

States A2a() {
    lineNumber++;

    return States::states_A2;
}

States A2b() {
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2c() {
    constantTable.insert(numberRegister);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2d() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2e() {
    if (relationRegister == '!')
        return ERROR1(lineNumber);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2f() {
    classRegister = static_cast<unsigned short>(LexemeClass::ERROR);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States B1a() {
    char currentSymbol = static_cast<char>(std::tolower(static_cast<int>(globalSymbol.value)));
    variableRegister = std::string(1, currentSymbol);

    auto it = initialMap.find(currentSymbol);
    if (it != initialMap.end()) {
        std::visit([]<typename T0>(T0 &&arg) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_arithmetic_v<T>)
                detectionRegister = static_cast<short>(arg);
            else
                detectionRegister = -1;
        }, it->second);
    }

    if (detectionRegister == 0)
        return ERROR1(lineNumber);

    return States::states_B1;
}

States B1b() {
    detectionRegister++;

    return States::states_B1;
}

States C1() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB) {
        return States::states_C1;
    } else if (globalSymbol.tokenClass == SymbolicTokenClass::END_OF_LINE ||
               globalSymbol.tokenClass == SymbolicTokenClass::SEMICOLON ||
               globalSymbol.tokenClass == SymbolicTokenClass::END_OF_FILE) {
        return States::states_A2;
    } else
        return ERROR1(lineNumber);
}

States C1a() {
    switch (static_cast<char>(globalSymbol.value)) {
        case '+':
            classRegister = static_cast<unsigned short>(LexemeClass::ADD);
            break;
        case '-':
            classRegister = static_cast<unsigned short>(LexemeClass::SUB);
            break;
        case '*':
            classRegister = static_cast<unsigned short>(LexemeClass::MUL);
            break;
        case '/':
            classRegister = static_cast<unsigned short>(LexemeClass::DIV);
            break;
        case '%':
            classRegister = static_cast<unsigned short>(LexemeClass::MOD);
            break;
        default:
            classRegister = static_cast<unsigned short>(LexemeClass::ERROR);
    }

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister), lineNumber);

    return States::states_C1;
}

States C1b() {
    classRegister = static_cast<unsigned short>(LexemeClass::END);
    createLexeme(static_cast<LexemeClass>(classRegister),
                 pointerRegister,
                 numberRegister,
                 0,
                 lineNumber);

    return States::states_C1;
}

States C1c() {
    classRegister = static_cast<unsigned short>(LexemeClass::READ);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister),
                 lineNumber
    );

    return States::states_C1;
}

States C1d() {
    classRegister = static_cast<unsigned short>(LexemeClass::WRITE);
    createLexeme(static_cast<LexemeClass>(classRegister),
                 pointerRegister,
                 numberRegister,
                 0,
                 lineNumber);

    return States::states_C1;
}

States C1e() {
    auto localPointerRegister = static_cast<unsigned short>(pointerRegister);
    addConstant(localPointerRegister, numberRegister, constantFlag);
    pointerRegister = localPointerRegister;

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister),
                 lineNumber
    );

    return States::states_C1;
}

States C1f() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister),
                 lineNumber
    );

    return States::states_C1;
}

States C1g() {
    if (relationRegister == '!')
        return ERROR1(lineNumber);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister,
                 static_cast<unsigned>(relationRegister),
                 lineNumber
    );

    return States::states_C1;
}

States C1h() {
    char currentSymbol = static_cast<char>(globalSymbol.value);
    short relationValue = processRelation(relationRegister, currentSymbol);

    if (relationValue == 0)
        return ERROR1(lineNumber);

    switch (relationValue) {
        case LESS_EQUAL_CODE:
            classRegister = static_cast<unsigned short>(LexemeClass::LESS_EQUAL);
            break;
        case GREATER_EQUAL_CODE:
            classRegister = static_cast<unsigned short>(LexemeClass::GREATER_EQUAL);
            break;
        case EQUAL_CODE:
            classRegister = static_cast<unsigned short>(LexemeClass::EQUAL);
            break;
        case NOT_EQUAL_CODE:
            classRegister = static_cast<unsigned short>(LexemeClass::NOT_EQUAL);
            break;
        default: ;
    }

    relationRegister = currentSymbol;

    createLexeme(
        static_cast<LexemeClass>(classRegister),
        pointerRegister,
        numberRegister,
        static_cast<unsigned>(relationValue),
        lineNumber
    );

    return States::states_C1;
}

States D1a() {
    relationRegister = static_cast<char>(globalSymbol.value);

    switch (relationRegister) {
        case '>':
            classRegister = static_cast<unsigned short>(LexemeClass::GREATER);
            break;
        case '<':
            classRegister = static_cast<unsigned short>(LexemeClass::LESS);
            break;
        case '=':
            classRegister = static_cast<unsigned short>(LexemeClass::EQUAL);
            break;
        case '!':
            classRegister = static_cast<unsigned short>(LexemeClass::NOT_EQUAL);
            break;
        default:
            return ERROR1(lineNumber);
    }

    return States::states_D1;
}

States E1a() {
    classRegister = static_cast<unsigned short>(LexemeClass::PUSH);
    constantFlag = true;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, 0, lineNumber);

    return States::states_E1;
}

States E2a() {
    classRegister = static_cast<unsigned short>(LexemeClass::JI);
    constantFlag = false;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, 0, lineNumber);

    return States::states_E2;
}

States E2b() {
    classRegister = static_cast<unsigned short>(LexemeClass::JMP);
    constantFlag = false;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, 0, lineNumber);

    return States::states_E2;
}

States E3a() {
    classRegister = static_cast<unsigned short>(LexemeClass::POP);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, 0, lineNumber);

    return States::states_E3;
}

extern States H1a();
extern States G1a();

States F1() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB)
        return States::states_F1;
    else if (globalSymbol.tokenClass == SymbolicTokenClass::LETTER)
        return H1a();
    else if (globalSymbol.tokenClass == SymbolicTokenClass::DIGIT)
        return G1a();
    else
        return ERROR1(lineNumber);
}

States F2() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB)
        return States::states_F2;
    else if (globalSymbol.tokenClass == SymbolicTokenClass::DIGIT)
        return G1a();
    else
        return ERROR1(lineNumber);
}

States F3() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB)
        return States::states_F3;
    else if (globalSymbol.tokenClass == SymbolicTokenClass::LETTER)
        return H1a();
    else
        return ERROR1(lineNumber);
}

States G1a() {
    numberRegister = globalSymbol.value;
    classRegister = static_cast<unsigned short>(LexemeClass::CONSTANT);

    return States::states_G1;
}

States G1b() {
    numberRegister = 10 * numberRegister + globalSymbol.value;

    return States::states_G1;
}

States H1a() {
    variableRegister = static_cast<char>(globalSymbol.value);
    classRegister = VARIABLE_CODE;
    variableRegister = std::string(1, static_cast<char>(globalSymbol.value));

    return States::states_H1;
}

extern States handleVCommand();

States H1b() {
    variableRegister += static_cast<char>(globalSymbol.value);

    if (variableRegister == "vadd") {
        classRegister = static_cast<unsigned short>(LexemeClass::VADD);
        createLexeme(LexemeClass::VADD, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vsub") {
        classRegister = static_cast<unsigned short>(LexemeClass::VSUB);
        createLexeme(LexemeClass::VSUB, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vmul") {
        classRegister = static_cast<unsigned short>(LexemeClass::VMUL);
        createLexeme(LexemeClass::VMUL, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vdiv") {
        classRegister = static_cast<unsigned short>(LexemeClass::VDIV);
        createLexeme(LexemeClass::VDIV, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vmod") {
        classRegister = static_cast<unsigned short>(LexemeClass::VMOD);
        createLexeme(LexemeClass::VMOD, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vdot") {
        classRegister = static_cast<unsigned short>(LexemeClass::VDOT);
        createLexeme(LexemeClass::VDOT, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vconcat") {
        classRegister = static_cast<unsigned short>(LexemeClass::VCONCAT);
        createLexeme(LexemeClass::VCONCAT, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vlshift") {
        classRegister = static_cast<unsigned short>(LexemeClass::VLSHIFT);
        createLexeme(LexemeClass::VLSHIFT, 0, 0, 0, lineNumber);
        return States::states_C1;
    }
    if (variableRegister == "vrshift") {
        classRegister = static_cast<unsigned short>(LexemeClass::VRSHIFT);
        createLexeme(LexemeClass::VRSHIFT, 0, 0, 0, lineNumber);
        return States::states_C1;
    }

    return States::states_H1;
}

States I1() {
    return States::states_I1;
}

States I1a() {
    classRegister = static_cast<unsigned short>(LexemeClass::COMMENT);
    globalComment.clear();

    return States::states_I1;
}

States I2() {
    globalComment += static_cast<char>(globalSymbol.value);

    return States::states_I2;
}

States I2a() {
    try {
        if (globalComment.empty() || globalComment[0] == ' ')
            classRegister = static_cast<unsigned short>(LexemeClass::COMMENT);
        else {
            if (std::ranges::all_of(globalComment, ::isdigit))
                classRegister = static_cast<unsigned short>(std::stoi(globalComment));
            else
                classRegister = static_cast<unsigned short>(LexemeClass::COMMENT);
        }
    } catch (const std::exception &e) {
        std::cerr << "ошибка при преобразовании строки в число: " << e.what() << std::endl;
        classRegister = static_cast<unsigned short>(LexemeClass::COMMENT);
    }

    return States::states_I2;
}

States I2b() {
    auto localPointerRegister = static_cast<unsigned short>(pointerRegister);
    addConstant(localPointerRegister, numberRegister, constantFlag);
    pointerRegister = localPointerRegister;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    classRegister = static_cast<unsigned short>(std::stoi(globalComment));

    return States::states_I2;
}

States I2c() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    classRegister = static_cast<unsigned short>(std::stoi(globalComment));

    return States::states_I2;
}

States I2d() {
    if (relationRegister == '!')
        return ERROR1(lineNumber);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    classRegister = static_cast<unsigned short>(std::stoi(globalComment));

    return States::states_I2;
}

States J1() {
    return States::states_J1;
}

States M1() {
    [[maybe_unused]] static size_t lastFoundIndex = 0;

    if (!globalSymbol.value)
        return ERROR1(lineNumber);

    char currentSymbol = static_cast<char>(std::tolower(static_cast<int>(globalSymbol.value)));

    for (const auto &[value, symbol, optionalValue, procedure]: vectorOfAlternatives) {
        if (static_cast<unsigned long>(detectionRegister) == value) {
            if (symbol == currentSymbol) {
                lastFoundIndex = 0;
                if (procedure) {
                    return procedure();
                }
            } else if (optionalValue.has_value()) {
                detectionRegister = static_cast<short>(optionalValue.value());
                return M1();
            }
        }
    }

    if (detectionRegister == 0)
        return ERROR1(lineNumber);

    return States::states_M1;
}

States V1() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::DIGIT) {
        numberRegister = globalSymbol.value;
        classRegister = static_cast<unsigned short>(LexemeClass::CONSTANT);
        return States::states_V2;
    }
    if (globalSymbol.tokenClass == SymbolicTokenClass::SPACE_OR_TAB)
        return States::states_V1;
    return ERROR1(lineNumber);
}

States V2() {
    if (globalSymbol.tokenClass == SymbolicTokenClass::DIGIT) {
        numberRegister = 10 * numberRegister + globalSymbol.value;
        return States::states_V2;
    }
    if (globalSymbol.tokenClass == SymbolicTokenClass::COMMA) {
        currentVector.push_back(numberRegister);
        createLexeme(static_cast<LexemeClass>(LexemeClass::CONSTANT), 0, numberRegister, 0, lineNumber);
        createLexeme(LexemeClass::COMMA, 0, 0, 0, lineNumber);
        numberRegister = 0;
        return States::states_V1;
    }
    if (globalSymbol.tokenClass == SymbolicTokenClass::VECTOR_SYMBOL &&
        globalSymbol.value == VECTOR_END_CODE) {
        currentVector.push_back(numberRegister);
        vectors.push_back(currentVector);
        currentVector.clear();

        createLexeme(static_cast<LexemeClass>(LexemeClass::CONSTANT), 0, numberRegister, 0, lineNumber);
        createLexeme(LexemeClass::VECTOR_END, 0, 0, 0, lineNumber);
        return States::states_C1;
        }
    return ERROR1(lineNumber);
}

States handleVCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VADD);
    createLexeme(LexemeClass::VADD, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVSubCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VSUB);
    createLexeme(LexemeClass::VSUB, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVMulCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VMUL);
    createLexeme(LexemeClass::VMUL, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVDivCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VDIV);
    createLexeme(LexemeClass::VDIV, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVModCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VMOD);
    createLexeme(LexemeClass::VMOD, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVDotCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VDOT);
    createLexeme(LexemeClass::VDOT, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVConcatCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VCONCAT);
    createLexeme(LexemeClass::VCONCAT, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVLShiftCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VLSHIFT);
    createLexeme(LexemeClass::VLSHIFT, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States handleVRShiftCommand() {
    classRegister = static_cast<unsigned short>(LexemeClass::VRSHIFT);
    createLexeme(LexemeClass::VRSHIFT, 0, 0, 0, lineNumber);
    return States::states_C1;
}

States EXIT1() {
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);

    return static_cast<States>(0);
}

States EXIT2() {
    if (relationRegister == '!')
        return ERROR1(lineNumber);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);

    return static_cast<States>(0);
}

States EXIT3() {
    auto localPointerRegister = static_cast<unsigned short>(pointerRegister);
    addConstant(localPointerRegister, numberRegister, constantFlag);
    pointerRegister = localPointerRegister;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);

    return static_cast<States>(0);
}

States EXIT4() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);

    return static_cast<States>(0);
}

States ERROR1(const unsigned &lineNumber) {
    classRegister = static_cast<unsigned short>(LexemeClass::ERROR);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    std::cerr << "Обнаружена ошибка в строке номер " << lineNumber << std::endl;

    return States::states_J1;
}

constexpr unsigned short numberStates = 27; // количество состояний
constexpr unsigned short numberClass = 12; // количество символьных лексем

using functionPointer = States (*)();
using TransitionTable = std::array<std::array<functionPointer, numberClass>, numberStates>;

std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure> > vectorOfAlternatives = {
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

std::map<char, VariantType> initialMap{
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

TransitionTable initializeTable() {
    TransitionTable table{};
    for (auto &row: table)
        std::ranges::fill(row, []() -> States { return ERROR1(0); });

    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = []() {
        if (variableRegister == "vadd") {
            classRegister = static_cast<unsigned short>(LexemeClass::VADD);
            createLexeme(LexemeClass::VADD, 0, 0, 0, lineNumber);
            return States::states_C1;
        }
        return B1a();
    };
    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::ARITHMETIC_OPERATION)] = C1a;
    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::COMPARISON_OPERATION)] = D1a;
    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = A1;
    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A1b;
    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I1a;

    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = B1a;
    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::ARITHMETIC_OPERATION)] = C1a;
    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::COMPARISON_OPERATION)] = D1a;
    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = A2;
    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2a;
    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I2a;
    table[static_cast<std::size_t>(States::states_A2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT1;

    table[static_cast<std::size_t>(States::states_B1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = M1;
    table[static_cast<std::size_t>(States::states_B1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_C1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = C1;
    table[static_cast<std::size_t>(States::states_C1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2a;
    table[static_cast<std::size_t>(States::states_C1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I2a;
    table[static_cast<std::size_t>(States::states_C1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT1;

    table[static_cast<std::size_t>(States::states_D1)][static_cast<std::size_t>(SymbolicTokenClass::COMPARISON_OPERATION)] = C1h;
    table[static_cast<std::size_t>(States::states_D1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = C1g;
    table[static_cast<std::size_t>(States::states_D1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2e;
    table[static_cast<std::size_t>(States::states_D1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I2d;
    table[static_cast<std::size_t>(States::states_D1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT2;

    table[static_cast<std::size_t>(States::states_E1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = F1;
    table[static_cast<std::size_t>(States::states_E1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_E2)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = F2;
    table[static_cast<std::size_t>(States::states_E2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_E3)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = F3;
    table[static_cast<std::size_t>(States::states_E3)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_F1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = H1a;
    table[static_cast<std::size_t>(States::states_F1)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = G1a;
    table[static_cast<std::size_t>(States::states_F1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = F1;
    table[static_cast<std::size_t>(States::states_F1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_F2)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = G1a;
    table[static_cast<std::size_t>(States::states_F2)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = F2;
    table[static_cast<std::size_t>(States::states_F2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_F3)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = H1a;
    table[static_cast<std::size_t>(States::states_F3)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = F3;
    table[static_cast<std::size_t>(States::states_F3)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2f;

    table[static_cast<std::size_t>(States::states_G1)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = G1b;
    table[static_cast<std::size_t>(States::states_G1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = C1e;
    table[static_cast<std::size_t>(States::states_G1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2c;
    table[static_cast<std::size_t>(States::states_G1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I2b;
    table[static_cast<std::size_t>(States::states_G1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT3;

    table[static_cast<std::size_t>(States::states_H1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = H1b;
    table[static_cast<std::size_t>(States::states_H1)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = H1b;
    table[static_cast<std::size_t>(States::states_H1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = C1f;
    table[static_cast<std::size_t>(States::states_H1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2d;
    table[static_cast<std::size_t>(States::states_H1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I2c;
    table[static_cast<std::size_t>(States::states_H1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT4;

    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = I1;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = I1;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::ARITHMETIC_OPERATION)] = I1;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::COMPARISON_OPERATION)] = I1;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = I1;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A1a;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I1;
    table[static_cast<std::size_t>(States::states_I1)][static_cast<std::size_t>(SymbolicTokenClass::ERROR)] = I1;

    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::ARITHMETIC_OPERATION)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::COMPARISON_OPERATION)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2b;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::ERROR)] = I2;
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT1;

    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::ARITHMETIC_OPERATION)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::COMPARISON_OPERATION)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_LINE)] = A2a;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::SEMICOLON)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::ERROR)] = J1;
    table[static_cast<std::size_t>(States::states_J1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT1;

    table[static_cast<std::size_t>(States::states_F1)][static_cast<std::size_t>(SymbolicTokenClass::VECTOR_SYMBOL)] = [](){
        createLexeme(LexemeClass::VECTOR_START, 0, 0, 0, lineNumber);
        return States::states_V1;
    };

    table[static_cast<std::size_t>(States::states_V1)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = V1;
    table[static_cast<std::size_t>(States::states_V2)][static_cast<std::size_t>(SymbolicTokenClass::COMMA)] = V2;
    table[static_cast<std::size_t>(States::states_V1)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = V1;

    table[static_cast<std::size_t>(States::states_V2)][static_cast<std::size_t>(SymbolicTokenClass::SPACE_OR_TAB)] = V2;
    table[static_cast<std::size_t>(States::states_V2)][static_cast<std::size_t>(SymbolicTokenClass::DIGIT)] = V2;
    table[static_cast<std::size_t>(States::states_V2)][static_cast<std::size_t>(SymbolicTokenClass::VECTOR_SYMBOL)] = V2;

    return table;
}

void parse(const std::string &filePath) {
    TransitionTable table = initializeTable();

    file.open(filePath);
    if (!file) {
        std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
        return;
    }

    std::cout << "Содержимое файла:\n";
    std::string fileContent((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    std::cout << fileContent << std::endl << std::endl;

    file.clear();
    file.seekg(0);

    char currentChar;
    States currentState = States::states_A1;

    while (currentState != States::states_STOP) {
        int charCode = file.get();

        if (charCode == EOF) {
            globalSymbol = transliterator(EOF);
            auto tokenClass = static_cast<size_t>(globalSymbol.tokenClass);
            auto stateIndex = static_cast<size_t>(currentState);

            if (stateIndex >= table.size() || tokenClass >= table[stateIndex].size()) {
                std::cerr << "Ошибка: неверные индексы доступа к таблице переходов." << std::endl;
                break;
            }

            functionPointer nextStateFunction = table[stateIndex][tokenClass];
            if (nextStateFunction)
                currentState = nextStateFunction();
            else
                std::cerr << "Ошибка: отсутствует функция перехода для EOF." << std::endl;
            break;
        }

        currentChar = static_cast<char>(charCode);
        globalSymbol = transliterator(currentChar);

        auto tokenClass = static_cast<size_t>(globalSymbol.tokenClass);
        auto stateIndex = static_cast<size_t>(currentState);

        if (stateIndex >= table.size() || tokenClass >= table[stateIndex].size()) {
            std::cerr << "Ошибка: неверные индексы доступа к таблице переходов." << std::endl;
            break;
        }

        functionPointer nextStateFunction = table[stateIndex][tokenClass];
        if (nextStateFunction)
            currentState = nextStateFunction();
        else
            break;
    }
    file.close();
}

#endif //LEXICALANALYZER_HPP
