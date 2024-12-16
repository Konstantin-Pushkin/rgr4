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
#include <variant>

std::map<std::string, unsigned> nameTable;

// значения некоторые лексем
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
    END_MARKER = END_MARKER_CODE
};

// список символьных лексем
enum class SymbolicTokenClass {
    LETTER, DIGIT, SIGN, ARITHMETIC_OPERATION, COMPARISON_OPERATION, SPACE_OR_TAB, END_OF_LINE, SEMICOLON,
    ERROR, END_OF_FILE
};

// состояния
enum class States {
    states_A1, states_A2, states_B1, states_C1, states_D1, states_E1, states_E2, states_E3, states_F1, states_F2,
    states_F3, states_G1, states_H1, states_I1, states_I2, states_J1, states_M1, states_STOP
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

    //Lexeme(LexemeClass lexCl, unsigned val, unsigned line) : lexemeClass(lexCl), value(val), lineNumber(line) {}
};

std::vector<Lexeme> lexemes;

struct ConstantEntry {
    unsigned value;  // значение константы
    unsigned short registerIndex;  // индекс в таблице констант

    ConstantEntry(unsigned val, unsigned short regIdx) : value(val), registerIndex(regIdx) {}
};

std::vector<ConstantEntry> constantTable;

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
        unsigned newIndex = nameTable.size();
        nameTable[name] = newIndex;
        pointerRegister = newIndex;
    }
}

void addConstant(unsigned short pointerRegister, unsigned numberRegister, int constantFlag) {
    if (constantFlag == 0)
        return;

    auto it = std::ranges::find_if(constantTable,
                                   [numberRegister](const ConstantEntry &entry) {
                                       return entry.value == numberRegister;
                                   });

    if (it == constantTable.end()) {
        unsigned short newIndex = constantTable.size();
        constantTable.emplace_back(numberRegister, newIndex);
        it = std::prev(constantTable.end());
    }

    pointerRegister = it->registerIndex;
}

extern void createLexeme(LexemeClass classRegister, unsigned pointerRegister, unsigned numberRegister,
                  unsigned relationRegister,
                  unsigned lineNumber);

States handlePush(const std::string& variableName) {
    classRegister = static_cast<unsigned short>(LexemeClass::PUSH);

    addNameToTable(variableName);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_P1;
}

States handlePop(const std::string& variableName) {
    classRegister = static_cast<unsigned short>(LexemeClass::POP);

    addNameToTable(variableName);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_P2;
}

SymbolicToken transliterator(int ch) {
    SymbolicToken symbol{};
    symbol.value = 0;

    if (isalpha(ch)) {
        symbol.tokenClass = SymbolicTokenClass::LETTER;
    } else if (isdigit(ch)) {
        symbol.tokenClass = SymbolicTokenClass::DIGIT;
        symbol.value = static_cast<unsigned>(ch - '0');
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') {
        symbol.tokenClass = SymbolicTokenClass::ARITHMETIC_OPERATION;
        symbol.value = static_cast<unsigned>(ch);;
    } else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
        symbol.tokenClass = SymbolicTokenClass::COMPARISON_OPERATION;
    } else if (ch == ' ' || ch == '\t') {
        symbol.tokenClass = SymbolicTokenClass::SPACE_OR_TAB;
    } else if (ch == '\n') {
        symbol.tokenClass = SymbolicTokenClass::END_OF_LINE;
    } else if (ch == EOF) {
        symbol.tokenClass = SymbolicTokenClass::END_OF_FILE;
    } else if (ch == ';') {
        symbol.tokenClass = SymbolicTokenClass::SEMICOLON;
    } else {
        symbol.tokenClass = SymbolicTokenClass::ERROR;
    }

    return symbol;
}

unsigned relationTable[4][4] = {
    {0, 0, 0, 0},
    {2, 0, 0, 0},
    {5, 0, 0, 0},
    {6, 0, 0, 0}
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

unsigned currentLine = 1;

SymbolicToken globalSymbol;
std::string globalComment;

using Procedure = States(*)();
using VariantType = std::variant<unsigned long, SymbolicTokenClass>;
extern std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure> > vectorOfAlternatives;
extern std::map<char, VariantType> initialMap;
std::vector<std::string> variables;

void addVariable() {
    static const std::array<std::string, 7> keyWords = {"push", "pop", "jmp", {"ji"}, {"read"}, {"write"}, {"end"}};
    for (const auto &keyWord : keyWords)
        if (variableRegister == keyWord) {
            std::cerr << "Имя переменной совпадает с одним из ключевых слов" << std::endl;
        }

    std::string valueString = std::to_string(valueRegister);
    auto it = variables.begin();
    while (it != variables.end()) {
        if (*it == variableRegister) {
            break;
        }
        ++it;
    }
}

void createLexeme(LexemeClass classRegister, unsigned pointerRegister, unsigned numberRegister,
                  unsigned relationRegister,
                  unsigned lineNumber
) {
    unsigned lexemeValue = 0;

    switch (classRegister) {
        case LexemeClass::ARITHMETIC_OPERATION:
        case LexemeClass::RELATION:
            lexemeValue = relationRegister;
            break;

        case LexemeClass::PUSH:
        case LexemeClass::POP:
        case LexemeClass::READ:
        case LexemeClass::WRITE:
        case LexemeClass::JI:
            lexemeValue = pointerRegister;
            break;

        case LexemeClass::END:
        case LexemeClass::END_MARKER:
        case LexemeClass::COMMENT:
        case LexemeClass::ERROR:
            lexemeValue = 0;
            break;

        default:
            lexemeValue = numberRegister;
            break;
    }

    Lexeme newLexeme{};
    newLexeme.lexemeClass = classRegister;
    newLexeme.value = lexemeValue;
    newLexeme.lineNumber = lineNumber;

    lexemes.emplace_back(newLexeme);
}

extern States ERROR1();

States A1a() {
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    lineNumber++;
}

States A1b() {
    lineNumber++;

    return States::states_A1;
}

States A2a() {
    lineNumber++;

    return States::states_A2;
}

States A2b() {
    addVariable();
    lineNumber++;
}

States A2c() {
    if (!constantFlag)
        return States::states_G1;

    bool constantExists = false;
    for (const auto &[value, symbol, optionalValue, procedure]: vectorOfAlternatives) {
        if (value == numberRegister) {
            constantExists = true;
            break;
        }
    }

    if (!constantExists)
        vectorOfAlternatives.emplace_back(numberRegister, 'x', std::nullopt, nullptr);


    for (size_t i = 0; i < vectorOfAlternatives.size(); i++) {
        if (std::get<0>(vectorOfAlternatives[i]) == numberRegister) {
            detectionRegister = static_cast<short>(i);
            break;
        }
    }

    return States::states_A2;
}

States A2d() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2e() {
    if (relationRegister == '!')
        return ERROR1();

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    lineNumber++;

    return States::states_A2;
}

States A2f() {
    classRegister = static_cast<unsigned short>(LexemeClass::ERROR);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    lineNumber++;

    return States::states_A2;
}

States B1a() {
    char currentSymbol = static_cast<char>(std::tolower(globalSymbol.value));

    auto it = initialMap.find(currentSymbol);
    if (it != initialMap.end())
        detectionRegister = std::get<unsigned long>(it->second);
    else
        return ERROR1();


    return States::states_B1;
}

States B1b() {
    detectionRegister++;

    return States::states_B1;
}

States C1a() {
    classRegister = static_cast<unsigned short>(SymbolicTokenClass::ARITHMETIC_OPERATION);
    numberRegister = globalSymbol.value;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1b() {
    classRegister = static_cast<unsigned short>(LexemeClass::END);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1c() {
    classRegister = static_cast<unsigned short>(LexemeClass::READ);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1d() {
    classRegister = static_cast<unsigned short>(LexemeClass::WRITE);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1e() {
    addConstant(pointerRegister, numberRegister, constantFlag);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1f() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1g() {
    if (relationRegister == '!')
        return ERROR1();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    return States::states_C1;
}

States C1h() {
    if (relationRegister == '!')
        return ERROR1();

    char currentSymbol = static_cast<char>(globalSymbol.value);
    short result = processRelation(relationRegister, currentSymbol);
    if (result == 0)
        return ERROR1();

    relationRegister = static_cast<char>(result);
    createLexeme();

    return States::states_C1;
}

States D1a() {
    if (relationRegister == '=')
        classRegister = static_cast<unsigned short>(LexemeClass::RELATION);
    else if (relationRegister == '!')
        classRegister = static_cast<unsigned short>(LexemeClass::RELATION);
    else if (relationRegister == '<')
        classRegister = static_cast<unsigned short>(LexemeClass::RELATION);
    else if (relationRegister == '>')
        classRegister = static_cast<unsigned short>(LexemeClass::RELATION);
    else
        classRegister = static_cast<unsigned short>(LexemeClass::ERROR);

    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);

    relationRegister = '\0';

    return States::states_D1;
}

States E1a() {
    classRegister = static_cast<unsigned short>(LexemeClass::PUSH);
    constantFlag = true;

    return States::states_E1;
}

States E2a() {
    classRegister = static_cast<unsigned short>(LexemeClass::JI);
    constantFlag = false;

    return States::states_E2;
}

States E2b() {
    classRegister = static_cast<unsigned short>(LexemeClass::JMP);
    constantFlag = false;

    return States::states_E2;
}

States E3a() {
    classRegister = static_cast<unsigned short>(LexemeClass::POP);

    return States::states_E3;
}

States G1a() {
    numberRegister = globalSymbol.value;

    return States::states_G1;
}

States G1b() {
    numberRegister = 10 * numberRegister + (globalSymbol.value - '0');

    return States::states_G1;
}

States H1a() {
    variableRegister = globalSymbol.value;

    return States::states_H1;
}

States H1b() {
    variableRegister += globalSymbol.value;

    return States::states_H1;
}

States I1a() {
    classRegister = std::stoi(globalComment);

    return States::states_I1;
}

States I2a() {
    classRegister = std::stoi(globalComment);

    return States::states_I2;
}

States I2b() {
    addConstant(pointerRegister, numberRegister, constantFlag);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    classRegister = std::stoi(globalComment);

    return States::states_I2;
}

States I2c() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    classRegister = std::stoi(globalComment);

    return States::states_I2;
}

States I2d() {
    if (relationRegister == '!')
        return ERROR1();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    classRegister = std::stoi(globalComment);

    return States::states_I2;
}

States M1() {
    char currentSymbol = static_cast<char>(std::tolower(globalSymbol.value));

    if (detectionRegister == static_cast<short>(currentSymbol)) {
        for (const auto &[value, symbol, optionalValue, procedure]: vectorOfAlternatives) {
            if (value == detectionRegister) {
                if (procedure)
                    return procedure();
                else
                    return ERROR1();
            }
        }
    } else {
        for (const auto &[value, symbol, optionalValue, procedure]: vectorOfAlternatives) {
            if (static_cast<char>(std::tolower(symbol)) == currentSymbol) {
                detectionRegister = value;
                if (optionalValue.has_value())
                    detectionRegister = optionalValue.value();
                break;
            }
        }
    }

    if (detectionRegister == 0)
        return ERROR1();

    return States::states_M1;
}

States EXIT1() {
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    exit(EXIT_SUCCESS);
}

States EXIT2() {
    if (relationRegister == '!')
        classRegister = static_cast<unsigned short>(SymbolicTokenClass::ERROR);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    exit(EXIT_SUCCESS);
}

States EXIT3() {
    addConstant(pointerRegister, numberRegister, constantFlag);
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    exit(EXIT_SUCCESS);
}

States EXIT4() {
    addVariable();
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    classRegister = END_MARKER_CODE;
    createLexeme(static_cast<LexemeClass>(classRegister), pointerRegister, numberRegister, relationRegister, lineNumber);
    exit(EXIT_SUCCESS);
}

States ERROR1(const unsigned &lineNumber) {
    classRegister = static_cast<unsigned short>(LexemeClass::ERROR);
    std::cerr << "Обнаружена ошибка в строке номер " << lineNumber << std::endl;

    return States::states_J1;
}

constexpr unsigned short numberStates = 16; // количество состояний
constexpr unsigned short numberClass = 9; // количество символьных лексем

using functionPointer = States (*)();
using TransitionTable = std::array<std::array<functionPointer, numberClass>, numberStates>;

std::vector<std::tuple<unsigned long, char, std::optional<unsigned long>, Procedure> > vectorOfAlternatives {
        {1, 'n', std::nullopt, B1b},
        {2, 'd', std::nullopt, C1b},
        {3, 'i', 4, nullptr, E2a},
        {4, 'm', std::nullopt, B1b},
        {5, 'p', std::nullopt, E2b},
        {6, 'o', 8, B1b},
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
        row.fill(ERROR1);

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
    table[static_cast<std::size_t>(States::states_D1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = Exit2;

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
    table[static_cast<std::size_t>(States::states_G1)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = Exit3;

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
    table[static_cast<std::size_t>(States::states_I2)][static_cast<std::size_t>(SymbolicTokenClass::END_OF_FILE)] = EXIT1;

    return table;
}

void parse(const std::string &filePath, const TransitionTable &table) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
        return;
    }

    char currentChar;
    States currentState = States::states_A1;

    while (currentState != States::states_STOP) {
        currentChar = file.get();

        if (file.eof())
            break;

        globalSymbol = transliterator(currentChar);

        functionPointer nextStateFunction = table[static_cast<std::size_t>(currentState)][static_cast<std::size_t>(
            globalSymbol.tokenClass)];

        if (nextStateFunction) {
            currentState = nextStateFunction();
        } else {
            std::cerr << "Ошибка перехода для символа: " << currentChar << " в состоянии " << static_cast<int>(
                currentState) << std::endl;
            break;
        }
    }

    file.close();
}

#endif //LEXICALANALYZER_HPP