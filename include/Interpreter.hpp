#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <stack>
#include <vector>
#include <map>
#include <sstream>
#include <ranges>

#include "LexicalAnalyzer.hpp"
#include "DArray.hpp"

class Interpreter {
    std::stack<std::variant<int, DArray>> stack;
    std::map<std::string, std::variant<int, DArray>> variables;
    std::vector<std::string> program;
    std::vector<DArray> vectors;
    size_t currentLine;
    size_t vectorIndex;

public:
    explicit Interpreter(const std::vector<std::string> &programLines);

    explicit Interpreter(const std::vector<std::string> &programLines,
                         const std::vector<std::vector<unsigned>> &vectorsData);

    void execute();

    void printStack() const;

    void printVariables() const;

    static std::vector<std::string> readFileIntoVector(const std::string &filePath);
};

#endif // INTERPRETER_HPP