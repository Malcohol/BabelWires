/**
 * Describes the steps to follow within a ValueTree to reach a particular ValueTreeNode.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Path/path.hpp>

#include <BabelWiresLib/Path/pathStep.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/identifierVisitable.hpp>
#include <Common/Log/debugLogger.hpp>
#include <Common/types.hpp>

#include <algorithm>
#include <type_traits>

babelwires::Path::Path() {}

babelwires::Path::Path(std::vector<PathStep> steps)
    : m_steps(std::move(steps)) {
    assert(std::none_of(m_steps.begin(), m_steps.end(), [](auto p) { return p.isNotAStep(); }) &&
           "Attempt to construct a path from a vector containing a non-step");
}

void babelwires::Path::pushStep(PathStep step) {
    assert(!step.isNotAStep() && "Attempt to push a non-step onto a path");
    m_steps.emplace_back(std::move(step));
}

void babelwires::Path::popStep() {
    assert(!m_steps.empty() && "You can't pop from an empty path");
    m_steps.pop_back();
}

std::ostream& babelwires::operator<<(std::ostream& os, const Path& p) {
    const IdentifierRegistry::ReadAccess identifierRegistry = IdentifierRegistry::read();
    const int numSteps = p.getNumSteps();
    if (numSteps) {
        const char* delimiter = "";
        for (int i = 0; i < numSteps; ++i) {
            os << delimiter;
            delimiter = s_pathDelimiterString;
            const PathStep step = p.getStep(i);
            step.writeToStreamReadable(os, *identifierRegistry);
        }
    } else {
        os << ".";
    }
    return os;
}

std::string babelwires::Path::serializeToString() const {
    std::ostringstream os;
    const char* delimiter = "";
    for (int i = 0; i < getNumSteps(); ++i) {
        os << delimiter;
        delimiter = s_pathDelimiterString;
        os << getStep(i).serializeToString();
    }
    return os.str();
}

babelwires::Path babelwires::Path::deserializeFromString(const std::string& pathString) {
    if (pathString.empty()) {
        return {};
    }

    Path path;

    int start = -1;
    do {
        ++start;
        const int next = pathString.find(s_pathDelimiter, start);
        const std::string stepString = pathString.substr(start, next - start);
        path.m_steps.emplace_back(PathStep::deserializeFromString(stepString));
        if (path.m_steps.back().isNotAStep()) {
            throw ParseException() << "Parsing a path encountered a step which is not a step";
        }
        start = next;
    } while (start != std::string::npos);

    return path;
}

int babelwires::Path::compare(const Path& other) const {
    auto it = m_steps.begin();
    auto oit = other.m_steps.begin();
    while (true) {
        const bool thisAtEnd = (it == m_steps.end());
        const bool otherAtEnd = (oit == other.m_steps.end());
        if (thisAtEnd) {
            return otherAtEnd ? 0 : -1;
        } else if (otherAtEnd) {
            return 1;
        } else if (*it < *oit) {
            return -1;
        } else if (*oit < *it) {
            return 1;
        }
        ++it;
        ++oit;
    }
    return 0;
}

bool babelwires::Path::operator==(const Path& other) const {
    return compare(other) == 0;
}

bool babelwires::Path::operator!=(const Path& other) const {
    return compare(other);
}

bool babelwires::Path::operator<(const Path& other) const {
    return compare(other) == -1;
}

bool babelwires::Path::operator<=(const Path& other) const {
    return compare(other) <= 0;
}

bool babelwires::Path::isPrefixOf(const Path& other) const {
    if (m_steps.size() > other.m_steps.size()) {
        return false;
    }
    const auto pair = std::mismatch(m_steps.begin(), m_steps.end(), other.m_steps.begin());
    return pair.first == m_steps.end();
}

bool babelwires::Path::isStrictPrefixOf(const Path& other) const {
    if (isPrefixOf(other)) {
        return m_steps.size() != other.m_steps.size();
    }
    return false;
}

unsigned int babelwires::Path::getNumSteps() const {
    return m_steps.size();
}

void babelwires::Path::truncate(unsigned int newNumSteps) {
    assert((newNumSteps <= m_steps.size()) && "You can only shrink with truncate");
    // Have to provide a fill value even though it is never used.
    m_steps.resize(newNumSteps, 0);
}

void babelwires::Path::removePrefix(unsigned int numSteps) {
    assert((numSteps <= m_steps.size()) && "Cannot remove that many steps");
    if (numSteps > 0) {
        m_steps.erase(m_steps.begin(), m_steps.begin() + numSteps);
    }
}

void babelwires::Path::append(const Path& subpath) {
    m_steps.insert(m_steps.end(), subpath.m_steps.begin(), subpath.m_steps.end());
}

babelwires::PathStep& babelwires::Path::getStep(unsigned int i) {
    assert((i < m_steps.size()) && "There is no ith step");
    return m_steps[i];
}

const babelwires::PathStep& babelwires::Path::getStep(unsigned int i) const {
    assert((i < m_steps.size()) && "There is no ith step");
    return m_steps[i];
}

const babelwires::PathStep& babelwires::Path::getLastStep() const {
    assert((m_steps.size() > 0) && "There are no steps.");
    return m_steps.back();
}

std::size_t babelwires::Path::getHash() const {
    // Arbitrary value.
    std::size_t hash = 0xa73be88;
    for (const auto& step : m_steps) {
        hash::mixInto(hash, step);
    }
    return hash;
}

void babelwires::Path::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& step : m_steps) {
        step.visitIdentifiers(visitor);
    }
}
