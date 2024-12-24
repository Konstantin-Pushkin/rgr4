#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <stack>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>

#include "lexicalAnalyzer.hpp"
#include "DArray.hpp"

class Interpreter {
    std::stack<std::variant<int, DArray>> stack;
    std::map<std::string, std::variant<int, DArray>> variables;
    std::vector<std::string> program;
    size_t currentLine;
    size_t vectorIndex;

public:
    Interpreter(const std::vector<std::string> &programLines)
        : program(programLines), currentLine(0), vectorIndex(0) {}

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

            try {
                if (command == "push") {
                    std::string value;
                    iss >> value;

                    if (value.size() >= 4 && value.substr(0, 2) == "<<") {
                        if (vectorIndex < vectors.size()) {
                            DArray arr(vectors[vectorIndex]);
                            stack.push(arr);
                            vectorIndex++;
                        } else {
                            std::cerr << "вектор не найден в таблице векторов" << std::endl;
                            return;
                        }
                    } else {
                        try {
                            int num = std::stoi(value);
                            stack.push(num);
                        } catch (const std::invalid_argument&) {
                            if (variables.find(value) != variables.end())
                                stack.push(variables[value]);
                            else {
                                std::cerr << "переменная " << value << " не найдена." << std::endl;
                                return;
                            }
                        }
                    }
                } else if (command == "pop") {
                    std::string variable;
                    iss >> variable;
                    if (stack.empty()) throw std::runtime_error("стек пуст");
                    variables[variable] = stack.top();
                    stack.pop();
                } else if (command == "read") {
                    std::string input;
                    std::getline(std::cin, input);
                    if (input.size() >= 4 && input.substr(0, 2) == "<<") {
                        std::stringstream ss(input);
                        DArray arr;
                        ss >> arr;
                        stack.push(arr);
                    } else
                        stack.push(std::stoi(input));
                } else if (command == "write") {
                    if (stack.empty()) throw std::runtime_error("стек пуст");
                    if (auto* arr = std::get_if<DArray>(&stack.top()))
                        std::cout << *arr << std::endl;
                    else
                        std::cout << std::get<int>(stack.top()) << std::endl;
                    stack.pop();
                } else if (command == "+" || command == "-" || command == "*" || command == "/" || command == "%") {
                    if (stack.size() < 2) throw std::runtime_error("недостаточно элементов в стеке");
                    auto b = std::get<int>(stack.top());
                    stack.pop();
                    auto a = std::get<int>(stack.top());
                    stack.pop();

                    int result;
                    if (command == "+") result = a + b;
                    else if (command == "-") result = a - b;
                    else if (command == "*") result = a * b;
                    else if (command == "/") result = a / b;
                    else result = a % b;
                    stack.push(result);
                } else if (command == "vadd" || command == "vsub" || command == "vmul" ||
                           command == "vdiv" || command == "vmod") {
                    if (stack.size() < 2) throw std::runtime_error("недостаточно элементов в стеке");
                    auto b = std::get<DArray>(stack.top());
                    stack.pop();
                    auto a = std::get<DArray>(stack.top());
                    stack.pop();

                    if (command == "vadd") stack.push(a + b);
                    else if (command == "vsub") stack.push(a - b);
                    else if (command == "vmul") stack.push(a * b);
                    else if (command == "vdiv") stack.push(a / b);
                    else stack.push(a % b);
                } else if (command == "vdot") {
                    if (stack.size() < 2) throw std::runtime_error("недостаточно элементов в стеке");
                    auto b = std::get<DArray>(stack.top());
                    stack.pop();
                    auto a = std::get<DArray>(stack.top());
                    stack.pop();
                    stack.push(std::variant<int, DArray>(a.dot(b)));
                } else if (command == "vconcat") {
                    if (stack.size() < 2) throw std::runtime_error("недостаточно элементов в стеке");
                    auto b = std::get<DArray>(stack.top());
                    stack.pop();
                    auto a = std::get<DArray>(stack.top());
                    stack.pop();
                    stack.push(a & b);
                } else if (command == "vlshift" || command == "vrshift") {
                    if (stack.size() < 2) throw std::runtime_error("недостаточно элементов в стеке");
                    auto shift = std::get<int>(stack.top());
                    stack.pop();
                    auto vec = std::get<DArray>(stack.top());
                    stack.pop();
                    stack.push(command == "vlshift" ? (vec << static_cast<unsigned>(shift)) : (vec >> static_cast<unsigned>(shift)));
                } else if (command == "<" || command == ">" || command == "<=" ||
                           command == ">=" || command == "=" || command == "!=") {
                    if (stack.size() < 2) throw std::runtime_error("недостаточно элементов в стеке");
                    auto b = std::get<int>(stack.top());
                    stack.pop();
                    auto a = std::get<int>(stack.top());
                    stack.pop();

                    bool result;
                    if (command == "<") result = a < b;
                    else if (command == ">") result = a > b;
                    else if (command == "<=") result = a <= b;
                    else if (command == ">=") result = a >= b;
                    else if (command == "=") result = a == b;
                    else result = a != b;

                    stack.push(result ? 1 : 0);
                } else if (command == "ji") {
                    int line;
                    iss >> line;
                    if (std::get<int>(stack.top()) > 0) {
                        currentLine = static_cast<size_t>(line - 1);
                        stack.pop();
                        continue;
                    }
                    stack.pop();
                } else if (command == "jmp") {
                    int line;
                    iss >> line;
                    currentLine = static_cast<size_t>(line - 1);
                    continue;
                } else if (command == "end")
                    break;
                else
                    throw std::runtime_error("неизвестная команда: " + command);
                currentLine++;
            } catch (const std::exception &e) {
                std::cerr << "Ошибка на строке " << currentLine + 1 << ": " << e.what() << std::endl;
                return;
            }
        }
    }

    void printStack() const {
        std::cout << "содержимое стека:" << std::endl;
        auto temp = stack;
        while (!temp.empty()) {
            if (auto* arr = std::get_if<DArray>(&temp.top()))
                std::cout << *arr << ' ';
            else
                std::cout << std::get<int>(temp.top()) << ' ';
            temp.pop();
        }
        std::cout << std::endl;
    }

    void printVariables() const {
        std::cout << "\nпеременные:" << std::endl;
        for (const auto &[name, value]: variables) {
            std::cout << name << " = ";
            if (auto* arr = std::get_if<DArray>(&value))
                std::cout << *arr;
            else
                std::cout << std::get<int>(value);
            std::cout << std::endl;
        }
    }
};
#endif // INTERPRETER_HPP
