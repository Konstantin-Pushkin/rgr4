#include "DArray.hpp"

#include <stdexcept>
#include <iostream>

void DArray::resize(const unsigned newCap) {
    const auto newData = new int[newCap];
    for (size_t i = 0; i < size; i++) {
        newData[i] = data[i];
    }

    delete[]data;
    data = newData;
    capacity = newCap;
}

DArray::DArray(const unsigned newCapacity) : data(new int[newCapacity]), size(0), capacity(newCapacity) {
}

DArray::DArray(const DArray &other) : data(new int[other.capacity]), size(other.size), capacity(other.capacity) {
    for (size_t i = 0; i < size; i++) {
        data[i] = other.data[i];
    }
}

DArray::~DArray() {
    delete[] data;
}

DArray DArray::operator+(const DArray &right) const {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    DArray res(size);
    res.size = size;
    for (size_t i = 0; i < size; i++)
        res.data[i] = data[i] + right.data[i];

    return res;
}

DArray &DArray::operator+=(const DArray &right) {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    for (size_t i = 0; i < size; i++)
        data[i] += right.data[i];

    return *this;
}

DArray DArray::operator-(const DArray &right) const {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    DArray res(size);
    res.size = size;
    for (size_t i = 0; i < size; i++)
        res.data[i] = data[i] - right.data[i];

    return res;
}

DArray &DArray::operator-=(const DArray &right) {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    for (size_t i = 0; i < size; i++)
        data[i] -= right.data[i];

    return *this;
}

DArray DArray::operator*(const DArray &right) const {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    DArray res(size);
    res.size = size;
    for (size_t i = 0; i < size; i++)
        res.data[i] = data[i] * right.data[i];

    return res;
}

DArray &DArray::operator*=(const DArray &right) {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    for (size_t i = 0; i < size; i++)
        data[i] *= right.data[i];

    return *this;
}

DArray DArray::operator/(const DArray &right) const {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    DArray res(size);
    res.size = size;
    for (size_t i = 0; i < size; i++) {
        if (right.data[i] == 0) {
            throw std::invalid_argument("Division by zero");
        }

        res.data[i] = data[i] / right.data[i];
    }

    return res;
}

DArray &DArray::operator/=(const DArray &right) {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    for (size_t i = 0; i < size; i++) {
        if (right.data[i] == 0) {
            throw std::invalid_argument("Division by zero");
        }

        data[i] /= right.data[i];
    }

    return *this;
}

DArray DArray::operator%(const DArray &right) const {
    if (size != right.size) {
        throw std::invalid_argument("Vector size mismatch");
    }

    DArray res(size);
    res.size = size;
    for (size_t i = 0; i < size; i++) {
        if (right.data[i] == 0) {
            throw std::invalid_argument("Division by zero");
        }

        res.data[i] = data[i] % right.data[i];
    }

    return res;
}

DArray &DArray::operator%=(const DArray &right) {
    if (size != right.size)
        throw std::invalid_argument("Vector size mismatch");

    for (size_t i = 0; i < size; i++) {
        if (right.data[i] == 0)
            throw std::invalid_argument("Division by zero");

        data[i] %= right.data[i];
    }

    return *this;
}

// скалярное произведение
int DArray::dot(const DArray &right) const {
    const DArray multiplied = *this * right; // поэлементное умножение векторов

    int result = 0;
    for (size_t i = 0; i < size; i++) {
        result += multiplied.data[i];
    }
    return result;
}

int &DArray::operator[](unsigned index) {
    if (index >= size)
        throw std::out_of_range("Index out of range");
    return data[index];
}

const int &DArray::operator[](unsigned index) const {
    if (index >= size)
        throw std::out_of_range("Index out of range");
    return data[index];
}

DArray &DArray::operator=(const DArray &right) {
    if (this == &right)
        return *this;

    delete[] data;

    size = right.size;
    capacity = right.capacity;
    data = new int[capacity];
    for (size_t i = 0; i < size; i++)
        data[i] = right.data[i];

    return *this;
}

void DArray::push_back(int value) {
    if (size >= capacity) {
        resize(capacity * 2);
    }
    data[size++] = value;
}

bool DArray::operator==(const DArray &right) const {
    if (size != right.size)
        return false;

    for (size_t i = 0; i < size; i++) {
        if (data[i] != right.data[i])
            return false;
    }

    return true;
}

bool DArray::operator!=(const DArray &right) const {
    return !(*this == right);
}

DArray DArray::operator&(const DArray &right) const {
    DArray res(size + right.size);

    for (size_t i = 0; i < size; i++) {
        res.data[i] = data[i];
    }

    for (size_t i = 0; i < right.size; i++) {
        res.data[size + i] = right.data[i];
    }

    res.size = size + right.size;

    return res;
}

DArray &DArray::operator&=(const DArray &right) {
    const unsigned newSize = size + right.size;

    if (newSize > capacity) {
        const auto newData = new int[newSize];

        for (size_t i = 0; i < size; i++) {
            newData[i] = data[i];
        }

        for (size_t i = 0; i < right.size; i++) {
            newData[size + i] = right.data[i];
        }

        delete[] data;
        data = newData;
        capacity = newSize;
    } else {
        for (size_t i = 0; i < right.size; i++) {
            data[size + i] = right.data[i];
        }
    }

    size = newSize;
    return *this;
}

DArray &DArray::operator<<(unsigned shift) {
    if (shift >= size) {
        for (size_t i = 0; i < size; i++)
            data[i] = 0;
    } else {
        for (size_t i = 0; i < size - shift; i++)
            data[i] = data[i + shift];
        for (size_t i = size - shift; i < size; i++)
            data[i] = 0;
    }

    return *this;
}

DArray &DArray::operator>>(unsigned shift) {
    if (shift >= size) {
        for (size_t i = 0; i < size; i++)
            data[i] = 0;
    } else {
        for (size_t i = size - 1; i >= shift; i--)
            data[i] = data[i - shift];

        for (size_t i = 0; i < shift; i++)
            data[i] = 0;
    }

    return *this;
}

std::ostream &operator<<(std::ostream &os, const DArray &arr) {
    os << "<< ";
    for (size_t i = 0; i < arr.size; i++) {
        os << arr.data[i];
        if (i < arr.size - 1)
            os << ", ";
    }
    os << " >>";
    return os;
}

std::istream &operator>>(std::istream &is, DArray &arr) {
    char ch;

    is >> ch;
    if (ch != '<') throw std::invalid_argument("Invalid format: Expected '<'");
    is >> ch;
    if (ch != '<') throw std::invalid_argument("Invalid format: Expected '<<'");

    int value; // Переменная для временного хранения введенного значения

    while (true) {
        is >> value;
        arr.push_back(value);

        is >> ch;
        if (ch != ',') {
            is.putback(ch); // Возвращаем символ обратно в поток
            break;
        }
    }

    is >> ch;
    if (ch != '>') throw std::invalid_argument("Invalid format: Expected '>'");

    is >> ch;
    if (ch != '>') throw std::invalid_argument("Invalid format: Expected '>>'");

    return is;
}

int main() {
    DArray dar(3);
    dar.push_back(150);
    dar.push_back(150);
    dar.push_back(150);

    DArray dar2(3);
    dar2.push_back(15);
    dar2.push_back(15);
    dar2.push_back(15);

    DArray dar3(5);
    std::cin >> dar3;

    std::cout << dar3 << std::endl << std::endl;

    dar3 << (2);
    std::cout << dar3 << std::endl;

    return EXIT_SUCCESS;
}
