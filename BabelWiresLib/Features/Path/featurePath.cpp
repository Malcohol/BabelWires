/**
 * Describes the steps to follow within a tree of features to reach a particular feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <BabelWiresLib/Features/Path/pathStep.hpp>
#include <BabelWiresLib/Features/compoundFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Log/debugLogger.hpp>
#include <Common/types.hpp>

#include <algorithm>
#include <type_traits>

babelwires::FeaturePath::FeaturePath() {}

babelwires::FeaturePath::FeaturePath(const Feature* feature) {
    std::vector<PathStep> steps;

    const Feature* current = feature;
    const CompoundFeature* parent = current->getOwner();
    while (parent) {
        steps.push_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() && "Feature with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwner();
    }

    m_steps.reserve(steps.size());
    std::move(steps.rbegin(), steps.rend(), std::back_inserter(m_steps));
}

babelwires::FeaturePath::FeaturePath(std::vector<PathStep> steps)
    : m_steps(std::move(steps)) {
        assert(std::none_of(m_steps.begin(), m_steps.end(), [](auto p) { return p.isNotAStep(); }) && "Attempt to construct a path from a vector containing a non-step");
    }

void babelwires::FeaturePath::pushStep(PathStep step) {
    assert(!step.isNotAStep() && "Attempt to push a non-step onto a path");
    m_steps.emplace_back(std::move(step));
}

void babelwires::FeaturePath::popStep() {
    assert(!m_steps.empty() && "You can't pop from an empty path");
    m_steps.pop_back();
}

std::ostream& babelwires::operator<<(std::ostream& os, const FeaturePath& p) {
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

std::string babelwires::FeaturePath::serializeToString() const {
    std::ostringstream os;
    const char* delimiter = "";
    for (int i = 0; i < getNumSteps(); ++i) {
        os << delimiter;
        delimiter = s_pathDelimiterString;
        os << getStep(i).serializeToString();
    }
    return os.str();
}

babelwires::FeaturePath babelwires::FeaturePath::deserializeFromString(const std::string& pathString) {
    if (pathString.empty()) {
        return {};
    }

    FeaturePath path;

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

namespace {

    template <typename T> T& followPath(T& start, const babelwires::FeaturePath& p, int& index) {
        if (index < p.getNumSteps()) {
            if (auto* compound = start.template as<babelwires::CompoundFeature>()) {
                T& child = compound->getChildFromStep(p.getStep(index));
                ++index;
                return followPath(child, p, index);
            } else {
                throw babelwires::ModelException() << "Tried to step into a non-compound feature";
            }
        } else {
            return start;
        }
    }

    template <typename T> T& followPath(T& start, const babelwires::FeaturePath& p) {
        int index = 0;
        try {
            return followPath(start, p, index);
        } catch (const std::exception& e) {
            throw babelwires::ModelException()
                << e.what() << "; when trying to follow step #" << index + 1 << " in path \"" << p << '\"';
        }
    }

} // namespace

babelwires::Feature& babelwires::FeaturePath::follow(Feature& start) const {
    return followPath<Feature>(start, *this);
}

const babelwires::Feature& babelwires::FeaturePath::follow(const Feature& start) const {
    return followPath<const Feature>(start, *this);
}

namespace {

    template <typename T> T* tryFollowPath(T* start, const babelwires::FeaturePath& p, int index = 0) {
        if (index < p.getNumSteps()) {
            if (auto* compound = start->template as<babelwires::CompoundFeature>()) {
                T* child = compound->tryGetChildFromStep(p.getStep(index));
                return tryFollowPath(child, p, index + 1);
            } else {
                return nullptr;
            }
        } else {
            return start;
        }
    }

} // namespace

babelwires::Feature* babelwires::FeaturePath::tryFollow(Feature& start) const {
    return tryFollowPath<Feature>(&start, *this);
}

const babelwires::Feature* babelwires::FeaturePath::tryFollow(const Feature& start) const {
    return tryFollowPath<const Feature>(&start, *this);
}

int babelwires::FeaturePath::compare(const FeaturePath& other) const {
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

bool babelwires::FeaturePath::operator==(const FeaturePath& other) const {
    return compare(other) == 0;
}

bool babelwires::FeaturePath::operator!=(const FeaturePath& other) const {
    return compare(other);
}

bool babelwires::FeaturePath::operator<(const FeaturePath& other) const {
    return compare(other) == -1;
}

bool babelwires::FeaturePath::operator<=(const FeaturePath& other) const {
    return compare(other) <= 0;
}

bool babelwires::FeaturePath::isPrefixOf(const FeaturePath& other) const {
    if (m_steps.size() > other.m_steps.size()) {
        return false;
    }
    const auto pair = std::mismatch(m_steps.begin(), m_steps.end(), other.m_steps.begin());
    return pair.first == m_steps.end();
}

bool babelwires::FeaturePath::isStrictPrefixOf(const FeaturePath& other) const {
    if (isPrefixOf(other)) {
        return m_steps.size() != other.m_steps.size();
    }
    return false;
}

unsigned int babelwires::FeaturePath::getNumSteps() const {
    return m_steps.size();
}

void babelwires::FeaturePath::truncate(unsigned int newNumSteps) {
    assert((newNumSteps <= m_steps.size()) && "You can only shrink with truncate");
    // Have to provide a fill value even though it is never used.
    m_steps.resize(newNumSteps, PathStep(0));
}

void babelwires::FeaturePath::removePrefix(unsigned int numSteps) {
    assert((numSteps <= m_steps.size()) && "Cannot remove that many steps");
    if (numSteps > 0) {
        m_steps.erase(m_steps.begin(), m_steps.begin() + numSteps);
    }
}

void babelwires::FeaturePath::append(const FeaturePath& subpath) {
    m_steps.insert(m_steps.end(), subpath.m_steps.begin(), subpath.m_steps.end());
}

babelwires::PathStep& babelwires::FeaturePath::getStep(unsigned int i) {
    assert((i < m_steps.size()) && "There is no ith step");
    return m_steps[i];
}

const babelwires::PathStep& babelwires::FeaturePath::getStep(unsigned int i) const {
    assert((i < m_steps.size()) && "There is no ith step");
    return m_steps[i];
}

const babelwires::PathStep& babelwires::FeaturePath::getLastStep() const {
    assert((m_steps.size() > 0) && "There are no steps.");
    return m_steps.back();
}

std::size_t babelwires::FeaturePath::getHash() const {
    // Arbitrary value.
    std::size_t hash = 0xa73be88;
    for (const auto& step : m_steps) {
        hash::mixInto(hash, step);
    }
    return hash;
}
