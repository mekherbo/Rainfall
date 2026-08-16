#include <cstring>
#include "N.hpp"

N::N(int val) {
    value = val;
}

void N::setAnnotation(char *param_1) {
    size_t __n = std::strlen(param_1);
    std::memcpy(reinterpret_cast<char*>(this) + 4, param_1, __n);
}

int N::operator+(N &param_1) {
    return this->value + param_1.value;
}

int N::operator-(N &param_1) {
    return this->value - param_1.value;
}