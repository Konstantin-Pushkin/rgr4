#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <stack>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>

class Interpreter {
    std::stack<int> stack;
    std::map<std::string, int> variables;
    std::vector<std::string> program;
    size_t currentLine;

public:
    Interpreter(const std::vector<std::string> &programLines)
        : program(programLines), currentLine(0) {}

    void execute() {
        while (currentLine < program.size()) {
            const std::string &line = program[currentLine];

            if (line.empty() || line[0] == ';') {
                currentLine++;
                continue;
            }

            std::istringstream iss(line);
            std::string command;
            iss >> command;

            if (command == "push") {
                std::string value;
                iss >> value;

                try {
                    int num = std::stoi(value);
                    stack.push(num);
                } catch (const std::invalid_argument &) {
                    if (nameTable.contains(value))
                        stack.push(static_cast<int>(nameTable[value]));
                    else {
                        std::cerr << "переменная " << value << " не найдена." << std::endl;
                        return;
                    }
                }
            } else if (command == "pop") {
                std::string variable;
                iss >> variable;

                if (stack.empty()) {
                    std::cerr << "стек пуст (pop))." << std::endl;
                    return;
                }

                int value = stack.top();
                stack.pop();

                if (nameTable.find(variable) != nameTable.end())
                    nameTable[variable] = static_cast<unsigned>(value);
                else {
                    std::cerr << "переменная " << variable << " не найдена" << std::endl;
                    return;
                }
            } else if (command == "+") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке (+)." << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a + b);
            } else if (command == "-") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке (-)." << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a - b);
            } else if (command == "*") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке (*)" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a * b);
            } else if (command == "/") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке (/)" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                if (b == 0) {
                    std::cerr << "деление на ноль (/)" << std::endl;
                    return;
                }
                stack.push(a / b);
            } else if (command == "%") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке (%)" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                if (b == 0) {
                    std::cerr << "деление на ноль (%)" << std::endl;
                    return;
                }
                stack.push(a % b);
            } else if (command == "read") {
                std::cout << "вход в read" << std::endl;
                int value;
                std::cout << "введите значение: ";
                std::cin >> value;
                stack.push(value);
                currentLine++;
                continue;
            } else if (command == "write") {
                if (stack.empty()) {
                    std::cerr << "стек пуст при WRITE." << std::endl;
                    return;
                }
                std::cout << stack.top() << std::endl;
                stack.pop();
            } else if (command == "end")
                break;
            else if (command == "ji") {
                int line;
                iss >> line;
                if (!stack.empty() && stack.top() > 0)
                    currentLine = static_cast<size_t>(line - 1);
                else
                    currentLine++;
                stack.pop();
                continue;
            } else if (command == "jmp") {
                int line;
                iss >> line;
                currentLine = static_cast<size_t>(line - 1);
                continue;
            } else if (command == "<") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке для операции <" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a < b ? 1 : 0);
            } else if (command == ">") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке для операции >" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a > b ? 1 : 0);
            } else if (command == "<=") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке для операции <=" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a <= b ? 1 : 0);
            } else if (command == ">=") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке для операции >=" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a >= b ? 1 : 0);
            } else if (command == "=") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке для операции =" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a == b ? 1 : 0);
            } else if (command == "!=") {
                if (stack.size() < 2) {
                    std::cerr << "недостаточно элементов в стеке для операции !=" << std::endl;
                    return;
                }
                int b = stack.top();
                stack.pop();
                int a = stack.top();
                stack.pop();
                stack.push(a != b ? 1 : 0);
            } else {
                std::cerr << "неизвестная команда: " << command << std::endl;
            }

            currentLine++;
        }
    }

    void printStack() const {
        std::cout << "содержимое стека:" << std::endl;
        std::stack<int> temp = stack;
        while (!temp.empty()) {
            std::cout << temp.top() << ' ';
            temp.pop();
        }
        std::cout << std::endl << std::endl;
    }

    void printVariables() const {
        std::cout << "переменные:" << std::endl;
        for (const auto &[name, value]: variables)
            std::cout << name << " = " << value << std::endl;
    }
};

#endif // INTERPRETER_HPP
