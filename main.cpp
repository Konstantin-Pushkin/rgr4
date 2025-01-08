#include "include/LexicalAnalyzer.hpp"
#include "include/Interpreter.hpp"
#include "include/DArray.hpp"

int main() {
    const std::string filePath = "../tests.txt";
    std::vector<std::string> program = Interpreter::readFileIntoVector(filePath);

    std::ifstream checkFile(filePath);
    if (!checkFile.is_open()) {
        std::cerr << "Не удалось открыть файл tests" << filePath << std::endl;
        return EXIT_FAILURE;
    }

    checkFile.close();

    try {
        Interpreter interpreter({});
        parse(filePath);
    } catch (const std::exception &e) {
        std::cerr << "Ошибка при разборе файла: " << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    std::cout << std::endl << "Результаты лексического анализа:\n";
    std::cout << "Найдено лексем: " << lexemes.size() << '\n';
    for (const auto &[lexemeClass, value, lineNumber]: lexemes) {
        std::cout << "Класс лексемы: " << static_cast<int>(lexemeClass)
                << ", значение: " << getLexemeValueString(lexemeClass, value)
                << ", строка: " << lineNumber << '\n';
    }

    std::cout << std::endl << "Таблица констант:\n";
    if (constantTable.empty())
        std::cout << "Константы не найдены" << std::endl;
    else {
        std::cout << "Найдено констант: " << constantTable.size() << '\n';
        for (const auto &value: constantTable)
            std::cout << "Значение: " << value << '\n';
    }

    std::cout << std::endl << "Таблица имен:\n";
    if (nameTable.empty())
        std::cout << "Имена не найдены" << std::endl;
    else {
        std::cout << "Найдено имен: " << nameTable.size() << '\n';
        for (const auto &name: nameTable | std::views::keys)
            std::cout << "Имя: " << name << '\n';
    }

    std::cout << std::endl << "Таблица векторов:\n";
    if (vectors.empty())
        std::cout << "Вектора не найдены" << std::endl;
    else {
        std::cout << "Найдено векторов: " << vectors.size() << '\n';
        for (auto &vector: vectors) {
            std::cout << "<<";
            for (size_t j = 0; j < vector.size(); ++j) {
                std::cout << vector[j];
                if (j < vector.size() - 1)
                    std::cout << ", ";
            }
            std::cout << ">>\n";
        }
    }

    try {
        Interpreter interpreter(program);
        std::cout << std::endl << "Запуск программы:\n";
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