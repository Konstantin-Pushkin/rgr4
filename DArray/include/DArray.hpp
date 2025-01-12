#ifndef DARRAY_HPP
#define DARRAY_HPP

#include <iosfwd>
#include <vector>

struct Node {
    int value; // Значение, хранящееся в узле
    Node *next; // Указатель на следующий узел
    Node *prev; // Указатель на предыдущий узел

    explicit Node(int val) : value(val), next(nullptr), prev(nullptr) {}
};

class DArray {
    unsigned size; // Текущий размер массива
    Node *head; // Указатель на первый узел
    Node *tail; // Указатель на последний узел

    static void checkVectorSize(const DArray &left, const DArray &right);

    static void checkDivisionByZero(const DArray &right);

    void copyFrom(const DArray &other);

    void clear();

    DArray &applyBinaryAssignment(const DArray &right, void (*op)(int &, int));

    DArray applyBinaryOperation(const DArray &right, int (*op)(int, int)) const;

public:
    DArray();

    DArray(const DArray &other);

    explicit DArray(const std::vector<unsigned> &vec);

    ~DArray();

    DArray operator+(const DArray &right) const;

    DArray &operator+=(const DArray &right);

    DArray operator-(const DArray &right) const;

    DArray &operator-=(const DArray &right);

    DArray operator*(const DArray &right) const;

    DArray &operator*=(const DArray &right);

    DArray operator/(const DArray &right) const;

    DArray &operator/=(const DArray &right);

    DArray operator%(const DArray &right) const;

    DArray &operator%=(const DArray &right);

    [[nodiscard]] int dot(const DArray &right) const; // скалярное произведение

    int &operator[](unsigned index);

    const int &operator[](unsigned index) const;

    DArray &operator=(const DArray &right);

    bool operator==(const DArray &right) const;

    bool operator!=(const DArray &right) const;

    DArray operator&(const DArray &right) const;

    DArray &operator&=(const DArray &right);

    DArray operator<<(unsigned shift) const;

    DArray operator>>(unsigned shift) const;

    DArray &operator<<=(unsigned shift);

    DArray &operator>>=(unsigned shift);

    class Iterator {
        Node *current;

    public:
        explicit Iterator(Node *node);

        int &operator*() const;

        Iterator &operator++(); // Префиксный инкремент
        Iterator operator++(int); // Постфиксный инкремент

        Iterator &operator--(); // Префиксный декремент
        Iterator operator--(int); // Постфиксный декремент

        bool operator==(const Iterator &other) const;

        bool operator!=(const Iterator &other) const;
    };

    [[nodiscard]] Iterator begin() const;

    [[nodiscard]] static Iterator end();

    friend std::ostream &operator<<(std::ostream &os, const DArray &arr);

    friend std::istream &operator>>(std::istream &is, DArray &arr);

    void push_back(int value);

    [[nodiscard]] unsigned getSize() const;
};

#endif //DARRAY_HPP
