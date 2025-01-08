#include "../include/LexicalAnalyzer.hpp"

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
        case LexemeClass::ARITHMETIC_OPERATION:
        case LexemeClass::RELATION: return {1, static_cast<char>(value)};
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
    if (!globalSymbol.value)
        return ERROR1(lineNumber);

    char currentSymbol = static_cast<char>(std::tolower(static_cast<int>(globalSymbol.value)));

    while (true) {
        for (const auto &[value, symbol, optionalValue, procedure]: vectorOfAlternatives) {
            if (static_cast<unsigned long>(detectionRegister) == value) {
                if (symbol == currentSymbol) {
                    if (procedure)
                        return procedure();
                } else if (optionalValue.has_value())
                    detectionRegister = static_cast<short>(optionalValue.value());
            }
        }

        if (detectionRegister == 0)
            return ERROR1(lineNumber);

        return States::states_M1;
    }
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

    if (globalSymbol.tokenClass == SymbolicTokenClass::VECTOR_SYMBOL && globalSymbol.value == VECTOR_END_CODE) {
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

States handleVectorStart() {
    createLexeme(LexemeClass::VECTOR_START, 0, 0, 0, lineNumber);

    return States::states_V1;
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

    return States::states_J1;
}

void parse(const std::string &filePath) {
    const TransitionTable table = initializeTable();

    file.open(filePath);
    if (!file) {
        std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
        return;
    }

    auto currentState = States::states_A1;

    while (currentState != States::states_STOP) {
        const int charCode = file.get();

        if (charCode == EOF) {
            globalSymbol = transliterator(EOF);
            auto tokenClass = static_cast<size_t>(globalSymbol.tokenClass);
            auto stateIndex = static_cast<size_t>(currentState);

            if (stateIndex >= table.size() || tokenClass >= table[stateIndex].size()) {
                std::cerr << "Ошибка: неверные индексы доступа к таблице переходов." << std::endl;
                break;
            }

            if (const functionPointer nextStateFunction = table[stateIndex][tokenClass])
                currentState = nextStateFunction();
            else
                std::cerr << "Ошибка: отсутствует функция перехода для EOF." << std::endl;
            break;
        }

        char currentChar = static_cast<char>(charCode);
        globalSymbol = transliterator(currentChar);

        const auto tokenClass = static_cast<size_t>(globalSymbol.tokenClass);
        const auto stateIndex = static_cast<size_t>(currentState);

        if (stateIndex >= table.size() || tokenClass >= table[stateIndex].size()) {
            std::cerr << "Ошибка: неверные индексы доступа к таблице переходов." << std::endl;
            break;
        }

        if (const functionPointer nextStateFunction = table[stateIndex][tokenClass])
            currentState = nextStateFunction();
        else
            break;
    }

    file.close();
}
