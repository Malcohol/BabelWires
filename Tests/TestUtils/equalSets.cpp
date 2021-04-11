#include "Tests/TestUtils/equalSets.hpp"

#include <algorithm>

bool testUtils::areEqualSets(const setOfStrings& a, const setOfStrings& b) {
    if (a.empty() && b.empty()) {
        return true;
    }

    setOfStrings sortedA = a;
    std::sort(sortedA.begin(), sortedA.end());

    setOfStrings sortedB = b;
    std::sort(sortedB.begin(), sortedB.end());

    if (std::search(sortedA.begin(), sortedA.end(), sortedB.begin(), sortedB.end()) == sortedA.end()) {
        return false;
    }

    if (std::search(sortedB.begin(), sortedB.end(), sortedA.begin(), sortedA.end()) == sortedB.end()) {
        return false;
    }

    return true;
};
