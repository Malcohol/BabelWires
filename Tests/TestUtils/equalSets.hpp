#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>

namespace testUtils {
    /// Assumes T is sortable.
    template <typename T> bool areEqualSets(const std::vector<T>& a, const std::vector<T>& b) {
        if (a.empty() && b.empty()) {
            return true;
        }

        std::vector<T> sortedA = a;
        std::sort(sortedA.begin(), sortedA.end());

        std::vector<T> sortedB = b;
        std::sort(sortedB.begin(), sortedB.end());

        if (std::search(sortedA.begin(), sortedA.end(), sortedB.begin(), sortedB.end()) == sortedA.end()) {
            return false;
        }

        if (std::search(sortedB.begin(), sortedB.end(), sortedA.begin(), sortedA.end()) == sortedB.end()) {
            return false;
        }

        return true;
    }

    template <typename T> bool unorderedAreEqualSets(const std::vector<T>& a, const std::vector<T>& b) {
        if (a.empty() && b.empty()) {
            return true;
        }
        std::unordered_set<T> aset;
        for (auto elemA : a) {
            aset.insert(elemA);
        }
        std::unordered_set<T> bset;
        for (auto elemB : b) {
            bset.insert(elemB);
        }
        return aset == bset;
    }
} // namespace testUtils
