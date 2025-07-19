/**
 * Some utilities for Unicode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <Common/Utilities/unicodeUtils.hpp>

#include <stack>
#include <cstdint>

void babelwires::writeUnicodeSubscript(std::ostream& os, int number) {
    if (number < 0)
    {
        os << "\u208B";
        number = -number;
    }
    std::stack<std::uint8_t> digits;
    do {
        digits.push(number % 10);
        number = number / 10; 
    } while (number > 0);
    do {
        switch (digits.top()) {
            case 0: os << "\u2080"; break;
            case 1: os << "\u2081"; break;
            case 2: os << "\u2082"; break;
            case 3: os << "\u2083"; break;
            case 4: os << "\u2084"; break;
            case 5: os << "\u2085"; break;
            case 6: os << "\u2086"; break;
            case 7: os << "\u2087"; break;
            case 8: os << "\u2088"; break;
            case 9: os << "\u2089"; break;
        }
        digits.pop();
    } while (!digits.empty());
}

void babelwires::writeUnicodeSuperscript(std::ostream& os, int number) {
    if (number < 0)
    {
        os << "\u207B";
        number = -number;
    }
    std::stack<std::uint8_t> digits;
    do {
        digits.push(number % 10);
        number = number / 10; 
    } while (number > 0);
    do {
        switch (digits.top()) {
            case 0: os << "\u2070"; break;
            case 1: os << "\u00B9"; break;
            case 2: os << "\u00B2"; break;
            case 3: os << "\u00B3"; break;
            case 4: os << "\u2074"; break;
            case 5: os << "\u2075"; break;
            case 6: os << "\u2076"; break;
            case 7: os << "\u2077"; break;
            case 8: os << "\u2078"; break;
            case 9: os << "\u2079"; break;
        }
        digits.pop();
    } while (!digits.empty());
}
