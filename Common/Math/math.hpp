#pragma once

namespace babelwires {

    template <typename T> T gcd(T a, T b) {
        if (a == 0) {
            return b;
        } else {
            return gcd(b % a, a);
        }
    }

    template <typename T> T lcm(T a, T b) {
        const T temp = gcd(a, b);

        return temp ? (a / temp * b) : 0;
    }

} // namespace babelwires