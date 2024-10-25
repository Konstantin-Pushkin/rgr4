#ifndef DARRAY_HPP
#define DARRAY_HPP

#include <iosfwd>

// Вариант №47
class DArray {
    int *data{nullptr};
    unsigned size = 0; // текущее количество элементов в массиве
    unsigned capacity = 0; // сколько элементов может вместить в себя массив

    void resize(unsigned newCap);

public:
    DArray() = default;

    explicit DArray(unsigned newCapacity);

    DArray(const DArray &other);

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

    DArray &operator<<(unsigned shift);

    DArray &operator>>(unsigned shift);

    friend std::ostream &operator<<(std::ostream &os, const DArray &arr);

    friend std::istream &operator>>(std::istream &is, DArray &arr);

    void push_back(int value);
};

#endif //DARRAY_HPP
