#include <iostream>

#include "../include/Interpreter.hpp"

void Interpreter::execute() {
    bool hasErrors = false;

        for (size_t line = 0; line < program.size(); line++) {
            const std::string &currentCmd = program[line];

            if (currentCmd.empty() || currentCmd[0] == ';')
                continue;

            std::istringstream iss(currentCmd);
            std::string command;
            iss >> command;

            if (command != "push" && command != "pop" && command != "read" &&
                command != "write" && command != "+" && command != "-" &&
                command != "*" && command != "/" && command != "%" &&
                command != "vadd" && command != "vsub" && command != "vmul" &&
                command != "vdiv" && command != "vmod" && command != "vdot" &&
                command != "vconcat" && command != "vlshift" && command != "vrshift" &&
                command != "<" && command != ">" && command != "<=" &&
                command != ">=" && command != "=" && command != "!=" &&
                command != "ji" && command != "jmp" && command != "end") {
                std::cerr << "Обнаружена ошибка в строке номер " << line + 1
                        << ": неизвестная команда " << command << '\n';
                hasErrors = true;
            }
        }

        std::cout << std::flush;

        if (hasErrors)
            return;

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
                            stack.emplace(arr);
                            vectorIndex++;
                        } else {
                            std::cerr << "Вектор не найден в таблице векторов" << std::endl;
                            return;
                        }
                    } else {
                        try {
                            int num = std::stoi(value);
                            stack.emplace(num);
                        } catch (const std::invalid_argument &) {
                            if (variables.contains(value))
                                stack.push(variables[value]);
                            else {
                                std::cerr << "Переменная " << value << " не найдена." << std::endl;
                                return;
                            }
                        }
                    }
                } else if (command == "pop") {
                    std::string variable;
                    iss >> variable;
                    if (stack.empty()) throw std::runtime_error("Стек пуст");
                    variables[variable] = stack.top();
                    stack.pop();
                } else if (command == "read") {
                    std::string input;
                    std::getline(std::cin, input);
                    if (input.size() >= 4 && input.substr(0, 2) == "<<") {
                        std::stringstream ss(input);
                        DArray arr;
                        ss >> arr;
                        stack.emplace(arr);
                    } else
                        stack.emplace(std::stoi(input));
                } else if (command == "write") {
                    if (stack.empty()) throw std::runtime_error("Стек пуст");
                    if (auto *arr = std::get_if<DArray>(&stack.top()))
                        std::cout << *arr << '\n';
                    else
                        std::cout << std::get<int>(stack.top()) << std::endl;
                    stack.pop();
                } else if (command == "+" || command == "-" || command == "*" || command == "/" || command == "%") {
                    if (stack.size() < 2) throw std::runtime_error("Недостаточно элементов в стеке");
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
                    stack.emplace(result);
                } else if (command == "vadd" || command == "vsub" || command == "vmul" ||
                           command == "vdiv" || command == "vmod") {
                    if (stack.size() < 2) throw std::runtime_error("Недостаточно элементов в стеке");
                    auto b = std::get<DArray>(stack.top());
                    stack.pop();
                    auto a = std::get<DArray>(stack.top());
                    stack.pop();
                    if (command == "vadd") stack.emplace(a + b);
                    else if (command == "vsub") stack.emplace(a - b);
                    else if (command == "vmul") stack.emplace(a * b);
                    else if (command == "vdiv") stack.emplace(a / b);
                    else stack.emplace(a % b);
                } else if (command == "vdot") {
                    if (stack.size() < 2) throw std::runtime_error("Недостаточно элементов в стеке");
                    auto b = std::get<DArray>(stack.top());
                    stack.pop();
                    auto a = std::get<DArray>(stack.top());
                    stack.pop();
                    stack.emplace(a.dot(b));
                } else if (command == "vconcat") {
                    if (stack.size() < 2) throw std::runtime_error("Недостаточно элементов в стеке");
                    auto b = std::get<DArray>(stack.top());
                    stack.pop();
                    auto a = std::get<DArray>(stack.top());
                    stack.pop();
                    stack.emplace(a & b);
                } else if (command == "vlshift" || command == "vrshift") {
                    if (stack.size() < 2) throw std::runtime_error("Недостаточно элементов в стеке");
                    auto shift = std::get<int>(stack.top());
                    stack.pop();
                    auto vec = std::get<DArray>(stack.top());
                    stack.pop();
                    stack.emplace(command == "vlshift"
                                      ? (vec << static_cast<unsigned>(shift))
                                      : (vec >> static_cast<unsigned>(shift)));
                } else if (command == "<" || command == ">" || command == "<=" ||
                           command == ">=" || command == "=" || command == "!=") {
                    if (stack.size() < 2) throw std::runtime_error("Недостаточно элементов в стеке");
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
                    stack.emplace(result ? 1 : 0);
                } else if (command == "ji") {
                    int tempLine;
                    iss >> tempLine;
                    if (std::get<int>(stack.top()) > 0) {
                        currentLine = static_cast<size_t>(tempLine - 1);
                        stack.pop();
                        continue;
                    }
                    stack.pop();
                } else if (command == "jmp") {
                    int tempLine;
                    iss >> tempLine;
                    currentLine = static_cast<size_t>(tempLine - 1);
                    continue;
                } else if (command == "end")
                    break;
                else
                    return;
                currentLine++;
            } catch (const std::exception &e) {
                std::cerr << "Ошибка на строке " << currentLine + 1 << ": " << e.what() << '\n';
                return;
            }
        }
}

void Interpreter::printStack() const {
    std::cout << "Содержимое стека:\n";
    auto temp = stack;
    while (!temp.empty()) {
        if (auto *arr = std::get_if<DArray>(&temp.top()))
            std::cout << *arr << ' ';
        else
            std::cout << std::get<int>(temp.top()) << ' ';
        temp.pop();
        std::cout << '\n';
    }
}

void Interpreter::printVariables() const {
    if (variables.empty())
        std::cout << std::endl << "Переменные отсутствуют" << std::endl;
    else {
        std::cout << std::endl << "Переменные:" << '\n';
        for (const auto &name: variables | std::views::keys)
            std::cout << name << '\n';
    }

    std::cout << std::flush;
}

Interpreter::Interpreter(const std::vector<std::string> &programLines) : program(programLines), currentLine(0),
                                                                         vectorIndex(0) {}

std::vector<std::string> Interpreter::readFileIntoVector(const std::string &filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла: " << filePath << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (const size_t commentPos = line.find(';'); commentPos != std::string::npos)
            line = line.substr(0, commentPos);

        line = line.erase(line.find_last_not_of(" \t") + 1);

        lines.push_back(line);
    }

    file.close();

    return lines;
}
