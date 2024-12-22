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
#include <variant>

std::map<std::string, unsigned> nameTable;

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
};

// список символьных лексем
enum class SymbolicTokenClass {
    LETTER, DIGIT, SIGN, ARITHMETIC_OPERATION, COMPARISON_OPERATION, SPACE_OR_TAB, END_OF_LINE, SEMICOLON,
    ERROR, END_OF_FILE
};

// состояния
enum class States {
    states_A1, states_A2, states_B1, states_C1, states_D1, states_E1, states_E2, states_E3, states_F1, states_F2,
    states_F3, states_G1, states_H1, states_I1, states_I2, states_J1, states_M1, states_STOP, EXIT1, EXIT2, EXIT3,
    EXIT4, ERROR1
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

    explicit ConstantEntry(unsigned val) : value(val) {}
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

SymbolicToken transliterator(int ch) {
    SymbolicToken symbol{};
    symbol.value = 0;

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
        symbol.tokenClass = SymbolicTokenClass::COMPARISON_OPERATION;
        symbol.value = static_cast<unsigned>(ch);
    } else if (ch == ' ' || ch == '\t')
        symbol.tokenClass = SymbolicTokenClass::SPACE_OR_TAB;
    else if (ch == '\n')
        symbol.tokenClass = SymbolicTokenClass::END_OF_LINE;
    else if (ch == EOF)
        symbol.tokenClass = SymbolicTokenClass::END_OF_FILE;
    else if (ch == ';')
        symbol.tokenClass = SymbolicTokenClass::SEMICOLON;
    else
        symbol.tokenClass = SymbolicTokenClass::ERROR;

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
    static const std::array<std::string, 7> keyWords = {"push", "pop", "jmp", "ji", "read", "write", "end"};
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
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2c() {
    constantTable.insert(numberRegister);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2d() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2e() {
    if (relationRegister == '!')
        return ERROR1(lineNumber);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2f() {
    classRegister = static_cast<unsigned short>(LexemeClass::ERROR);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, static_cast<unsigned>(relationRegister), lineNumber);
    lineNumber++;

    return States::states_A2;
}

States B1a() {
    char currentSymbol = static_cast<char>(std::tolower(static_cast<int>(globalSymbol.value)));

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

    switch(relationValue) {
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

States H1b() {
    variableRegister += static_cast<char>(globalSymbol.value);

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
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при преобразовании строки в число: " << e.what() << std::endl;
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
    static size_t lastFoundIndex = 0;

    if (!globalSymbol.value)
        return ERROR1(lineNumber);

    char currentSymbol = static_cast<char>(std::tolower(static_cast<int>(globalSymbol.value)));

    if (detectionRegister == static_cast<short>(currentSymbol)) {
        for (const auto &[value, symbol, optionalValue, procedure]: vectorOfAlternatives) {
            if (static_cast<short>(value) == detectionRegister) {
                lastFoundIndex = 0;
                if (procedure)
                    return procedure();
                else
                    return ERROR1(lineNumber);
            }
        }
    } else {
        for (size_t i = lastFoundIndex; i < vectorOfAlternatives.size(); ++i) {
            const auto &[value, symbol, optionalValue, procedure] = vectorOfAlternatives[i];
            if (static_cast<char>(std::tolower(symbol)) == currentSymbol) {
                detectionRegister = static_cast<short>(value);
                if (optionalValue.has_value())
                    detectionRegister = static_cast<short>(optionalValue.value());

                lastFoundIndex = i + 1;

                if (procedure == C1d || procedure == C1c || procedure == E1a ||
                    procedure == E3a || procedure == E2b || procedure == E2a || procedure == C1b)
                    lastFoundIndex = 0;

                if (procedure)
                    return procedure();
                break;
            }
        }

        if (lastFoundIndex > 0) {
            for (size_t i = 0; i < lastFoundIndex && i < vectorOfAlternatives.size(); ++i) {
                const auto &[value, symbol, optionalValue, procedure] = vectorOfAlternatives[i];
                if (static_cast<char>(std::tolower(symbol)) == currentSymbol) {
                    detectionRegister = static_cast<short>(value);
                    if (optionalValue.has_value())
                        detectionRegister = static_cast<short>(optionalValue.value());

                    lastFoundIndex = i + 1;

                    if (procedure == C1d || procedure == C1c || procedure == E1a ||
                        procedure == E3a || procedure == E2b || procedure == E2a || procedure == C1b)
                        lastFoundIndex = 0;

                    if (procedure)
                        return procedure();
                    break;
                }
            }
        }
    }

    if (detectionRegister == 0)
        return ERROR1(lineNumber);

    return States::states_M1;
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

constexpr unsigned short numberStates = 17; // количество состояний
constexpr unsigned short numberClass = 10; // количество символьных лексем

using functionPointer = States (*)();
using TransitionTable = std::array<std::array<functionPointer, numberClass>, numberStates>;

std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure>> vectorOfAlternatives = {
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
    {17, 'e', std::nullopt, C1d}
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
    {'v', VariantType{SymbolicTokenClass::ERROR}},
    {'w', VariantType{static_cast<unsigned long>(14)}},
    {'x', VariantType{SymbolicTokenClass::ERROR}},
    {'y', VariantType{SymbolicTokenClass::ERROR}},
    {'z', VariantType{SymbolicTokenClass::ERROR}}
};

TransitionTable initializeTable() {
    TransitionTable table{};
    for (auto &row: table)
        std::ranges::fill(row, []() -> States { return ERROR1(0); });

    table[static_cast<std::size_t>(States::states_A1)][static_cast<std::size_t>(SymbolicTokenClass::LETTER)] = B1a;
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

    return table;
}

void parse(const std::string &filePath) {
    TransitionTable table = initializeTable();

    std::ifstream file(filePath);
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

        [[maybe_unused]]auto temp = currentState;
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
