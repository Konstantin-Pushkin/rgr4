#include "DArray.hpp"
#include "lexicalAnalyzer.hpp"
#include "interpreter.hpp"
#include <stdexcept>
#include <iostream>

void DArray::checkVectorSize(const DArray &left, const DArray &right) {
    if (left.size != right.size) // исправление: left.getSize() заменено на left.size
        throw std::invalid_argument("Несоответствие размера вектора");
}

void DArray::checkDivisionByZero(const DArray &right) {
    for (Node *current = right.head; current; current = current->next) {
        if (current->value == 0)
            throw std::invalid_argument("Деление на ноль");
    }
}

void DArray::copyFrom(const DArray &other) {
    for (Node *current = other.head; current != nullptr; current = current->next)
        push_back(current->value);
}

void DArray::clear() {
    while (head) {
        Node *temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    size = 0;
}

DArray &DArray::applyBinaryAssignment(const DArray &right, void (*op)(int &, int)) {
    checkVectorSize(*this, right);
    Node *leftCurrent = head;
    Node *rightCurrent = right.head;
    while (leftCurrent) {
        op(leftCurrent->value, rightCurrent->value);
        leftCurrent = leftCurrent->next;
        rightCurrent = rightCurrent->next;
    }

    return *this;
}

DArray DArray::applyBinaryOperation(const DArray &right, int (*op)(int, int)) const {
    checkVectorSize(*this, right);
    DArray result;
    Node *leftCurrent = head;
    Node *rightCurrent = right.head;
    while (leftCurrent) {
        result.push_back(op(leftCurrent->value, rightCurrent->value));
        leftCurrent = leftCurrent->next;
        rightCurrent = rightCurrent->next;
    }
    return result;
}

DArray::DArray() : head(nullptr), tail(nullptr), size(0) {
}

DArray::DArray(const DArray &other) : head(nullptr), tail(nullptr), size(0) {
    copyFrom(other);
}

DArray::~DArray() { clear(); }

DArray DArray::operator+(const DArray &right) const {
    return applyBinaryOperation(right, [](int a, int b) { return a + b; });
}

DArray &DArray::operator+=(const DArray &right) {
    applyBinaryAssignment(right, [](int &a, int b) { a += b; });
    return *this;
}

DArray DArray::operator-(const DArray &right) const {
    return applyBinaryOperation(right, [](int a, int b) { return a - b; });
}

DArray &DArray::operator-=(const DArray &right) {
    applyBinaryAssignment(right, [](int &a, int b) { a -= b; });
    return *this;
}

DArray DArray::operator*(const DArray &right) const {
    return applyBinaryOperation(right, [](int a, int b) { return a * b; });
}

DArray &DArray::operator*=(const DArray &right) {
    applyBinaryAssignment(right, [](int &a, int b) { a *= b; });
    return *this;
}

DArray DArray::operator/(const DArray &right) const {
    checkDivisionByZero(right);
    return applyBinaryOperation(right, [](int a, int b) { return a / b; });
}

DArray &DArray::operator/=(const DArray &right) {
    checkDivisionByZero(right);
    applyBinaryAssignment(right, [](int &a, int b) { a /= b; });
    return *this;
}

DArray DArray::operator%(const DArray &right) const {
    checkDivisionByZero(right);
    return applyBinaryOperation(right, [](int a, int b) { return a % b; });
}

DArray &DArray::operator%=(const DArray &right) {
    checkDivisionByZero(right);
    applyBinaryAssignment(right, [](int &a, int b) { a %= b; });
    return *this;
}

int DArray::dot(const DArray &right) const {
    checkVectorSize(*this, right);
    int result = 0;
    Node *leftCurrent = head;
    Node *rightCurrent = right.head;
    while (leftCurrent) {
        result += leftCurrent->value * rightCurrent->value;
        leftCurrent = leftCurrent->next;
        rightCurrent = rightCurrent->next;
    }
    return result;
}

int &DArray::operator[](unsigned index) {
    if (index >= size)
        throw std::out_of_range("Индекс вне диапазона");
    Node *current = head;
    for (size_t i = 0; i < index; i++)
        current = current->next;
    return current->value;
}

const int &DArray::operator[](unsigned index) const {
    if (index >= size)
        throw std::out_of_range("Индекс вне диапазона");
    Node *current = head;
    for (size_t i = 0; i < index; i++)
        current = current->next;
    return current->value;
}

DArray &DArray::operator=(const DArray &right) {
    if (this != &right) {
        clear();
        copyFrom(right);
    }
    return *this;
}

bool DArray::operator==(const DArray &right) const {
    if (size != right.size)
        return false;
    Node *leftCurrent = head;
    Node *rightCurrent = right.head;
    while (leftCurrent) {
        if (leftCurrent->value != rightCurrent->value)
            return false;
        leftCurrent = leftCurrent->next;
        rightCurrent = rightCurrent->next;
    }
    return true;
}

bool DArray::operator!=(const DArray &right) const {
    return !(*this == right);
}

DArray DArray::operator&(const DArray &right) const {
    DArray res;
    Node *leftCurrent = head;
    while (leftCurrent) {
        res.push_back(leftCurrent->value);
        leftCurrent = leftCurrent->next;
    }
    Node *rightCurrent = right.head;
    while (rightCurrent) {
        res.push_back(rightCurrent->value);
        rightCurrent = rightCurrent->next;
    }
    return res;
}

DArray &DArray::operator&=(const DArray &right) {
    Node *rightCurrent = right.head;
    while (rightCurrent) {
        push_back(rightCurrent->value);
        rightCurrent = rightCurrent->next;
    }
    return *this;
}

DArray DArray::operator<<(unsigned shift) const {
    DArray newArray;

    if (shift >= size) {
        for (size_t i = 0; i < size; ++i)
            newArray.push_back(0);
    } else {
        Node *current = head;
        for (size_t i = 0; i < shift; ++i)
            current = current->next;
        for (size_t i = 0; i < size - shift; ++i) {
            newArray.push_back(current->value);
            current = current->next;
        }
        for (size_t i = size - shift; i < size; ++i)
            newArray.push_back(0);
    }

    return newArray;
}

DArray DArray::operator>>(unsigned shift) const {
    DArray newArray;
    if (shift >= size)
        for (size_t i = 0; i < size; ++i)
            newArray.push_back(0);
    else {
        for (size_t i = 0; i < shift; ++i)
            newArray.push_back(0);
        Node *current = head;
        for (size_t i = 0; i < size - shift; ++i) {
            newArray.push_back(current->value);
            current = current->next;
        }
    }
    return newArray;
}

DArray &DArray::operator<<=(unsigned shift) {
    if (shift >= size) {
        Node *current = head;
        while (current != nullptr) {
            current->value = 0;
            current = current->next;
        }
    } else {
        Node *newHead = head;
        for (size_t i = 0; i < shift; ++i) {
            if (newHead != nullptr)
                newHead = newHead->next;
        }
        Node *oldHead = head;
        while (newHead != nullptr) {
            oldHead->value = newHead->value;
            oldHead = oldHead->next;
            newHead = newHead->next;
        }
        while (oldHead != nullptr) {
            oldHead->value = 0;
            oldHead = oldHead->next;
        }
    }

    return *this;
}

DArray &DArray::operator>>=(unsigned shift) {
    if (shift >= size) {
        Node *current = head;
        while (current) {
            current->value = 0;
            current = current->next;
        }
    } else {
        Node *current = tail;
        for (size_t i = size - 1; i >= shift; --i) {
            current->value = current->prev ? current->prev->value : 0;
            current = current->prev;
        }

        current = head;
        for (size_t i = 0; i < shift; ++i) {
            current->value = 0;
            current = current->next;
        }
    }

    return *this;
}

DArray::Iterator::Iterator(Node *node) : current(node) {
}

int &DArray::Iterator::operator*() const {
    return current->value;
}

DArray::Iterator &DArray::Iterator::operator++() {
    if (current)
        current = current->next;
    return *this;
}

DArray::Iterator DArray::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

DArray::Iterator &DArray::Iterator::operator--() {
    if (current)
        current = current->prev;
    return *this;
}

DArray::Iterator DArray::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

bool DArray::Iterator::operator==(const Iterator &other) const {
    return current == other.current;
}

bool DArray::Iterator::operator!=(const Iterator &other) const {
    return current != other.current;
}

DArray::Iterator DArray::begin() const {
    return Iterator(head);
}

DArray::Iterator DArray::end() {
    return Iterator(nullptr);
}

std::ostream &operator<<(std::ostream &os, const DArray &arr) {
    os << "<< ";
    for (auto it = arr.begin(); it != DArray::end(); ++it) {
        if (it != arr.begin())
            os << ", ";
        os << *it;
    }
    os << " >>";
    return os;
}

std::istream &operator>>(std::istream &is, DArray &arr) {
    arr.clear();
    char ch;
    if (!(is >> ch) || ch != '<' || !(is >> ch) || ch != '<')
        throw std::invalid_argument("ожидается '<<' в начале");
    int value;
    while (is >> value) {
        arr.push_back(value);
        if (is >> ch && ch == '>')
            break;
        else if (ch != ',')
            throw std::invalid_argument("ожидается ',' или '>>' после значений");
    }
    if (ch != '>' || !(is >> ch) || ch != '>')
        throw std::invalid_argument("ожидается '>>' в конце");
    return is;
}

void DArray::push_back(int value) {
    auto newNode = new Node(value);
    if (!tail)
        head = newNode;
    else {
        tail->next = newNode;
        newNode->prev = tail;
    }
    tail = newNode;
    ++size;
}

unsigned DArray::getSize() const {
    return size;
}

// int main() {
//     const std::string filePath = "../tests.txt";
//
//     std::ifstream checkFile(filePath);
//     if (!checkFile.is_open()) {
//         std::cerr << "не удалось открыть файл tests" << filePath << std::endl;
//         return EXIT_FAILURE;
//     }
//     checkFile.close();
//
//     try {
//         parse(filePath);
//     } catch (const std::exception& e) {
//         std::cerr << "ошибка при разборе файла: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
//
//     std::cout << "\nРезультаты лексического анализа:\n";
//     std::cout << "Найдено лексем: " << lexemes.size() << std::endl;
//     for (const auto& lexeme : lexemes) {
//         std::cout << "Класс лексемы: " << static_cast<int>(lexeme.lexemeClass)
//                   << ", значение: " << getLexemeValueString(lexeme.lexemeClass, lexeme.value)
//                   << ", строка: " << lexeme.lineNumber << std::endl;
//     }
//
//     std::cout << "\nТаблица констант:\n";
//     if (constantTable.empty())
//         std::cout << "Константы не найдены\n";
//     else {
//         std::cout << "Найдено констант: " << constantTable.size() << "\n";
//         for (const auto& value : constantTable)
//             std::cout << "Значение: " << value << std::endl;
//     }
//
//     std::cout << "\nТаблица имен:\n";
//     if (nameTable.empty())
//         std::cout << "Имена не найдены\n";
//     else {
//         std::cout << "Найдено имен: " << nameTable.size() << "\n";
//         for (const auto &[name, index] : nameTable)
//             std::cout << "Имя: " << name << ", индекс: " << index << std::endl;
//     }
//
//     try {
//         Interpreter interpreter(lexemes, nameTable);
//
//         std::cout << "\nЗапуск программы:\n";
//         interpreter.execute();
//
//         std::cout << "\nСостояние после выполнения:\n";
//         interpreter.printStack();
//         interpreter.printVariables();
//     } catch (const std::exception& e) {
//         std::cerr << "Ошибка при выполнении интерпретации: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
//
//     return EXIT_SUCCESS;
// }

std::vector<std::string> readFileIntoVector(const std::string &filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла: " << filePath << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);

        line = line.erase(line.find_last_not_of(" \t") + 1);

        lines.push_back(line);
    }

    file.close();
    return lines;
}

int main() {
    const std::string filePath = "../tests.txt";
    std::vector<std::string> program = readFileIntoVector(filePath);

    std::ifstream checkFile(filePath);
    if (!checkFile.is_open()) {
        std::cerr << "не удалось открыть файл tests" << filePath << std::endl;
        return EXIT_FAILURE;
    }
    checkFile.close();

    try {
        Interpreter interpreter({});

        parse(filePath);
    } catch (const std::exception &e) {
        std::cerr << "ошибка при разборе файла: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\nРезультаты лексического анализа:\n";
    std::cout << "Найдено лексем: " << lexemes.size() << std::endl;
    for (const auto &lexeme: lexemes) {
        std::cout << "Класс лексемы: " << static_cast<int>(lexeme.lexemeClass)
                << ", значение: " << getLexemeValueString(lexeme.lexemeClass, lexeme.value)
                << ", строка: " << lexeme.lineNumber << std::endl;
    }

    std::cout << "\nТаблица констант:\n";
    if (constantTable.empty())
        std::cout << "Константы не найдены\n";
    else {
        std::cout << "Найдено констант: " << constantTable.size() << "\n";
        for (const auto &value: constantTable)
            std::cout << "Значение: " << value << std::endl;
    }

    std::cout << "\nТаблица имен:\n";
    if (nameTable.empty())
        std::cout << "Имена не найдены\n";
    else {
        std::cout << "Найдено имен: " << nameTable.size() << "\n";
        for (const auto &[name, index]: nameTable) {
            std::cout << "Имя: " << name << std::endl;
        }
    }

    try {
        Interpreter interpreter(program);

        std::cout << "\nЗапуск программы:\n";
        interpreter.execute();

        std::cout << "\nСостояние после выполнения:\n";
        interpreter.printStack();
        interpreter.printVariables();
    } catch (const std::exception &e) {
        std::cerr << "Ошибка при выполнении интерпретации: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
