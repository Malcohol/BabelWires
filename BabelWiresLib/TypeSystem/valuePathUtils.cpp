/**
 * Functions for exploring values with Paths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <span>

namespace {

    std::optional<std::tuple<const babelwires::Type&, const babelwires::ValueHolder&>>
    tryFollow(const babelwires::TypeSystem& typeSystem, const babelwires::Type& type,
              const babelwires::ValueHolder& valueHolder, const babelwires::Path& p, int& index) {
        if (index < p.getNumSteps()) {
            if (const auto* compoundType = type.as<babelwires::CompoundType>()) {
                const int childIndex = compoundType->getChildIndexFromStep(valueHolder, p.getStep(index));
                if (childIndex >= 0) {
                    auto [childValue, _, childType] = compoundType->getChild(valueHolder, childIndex);
                    ++index;
                    return tryFollow(typeSystem, *childType, *childValue, p, index);
                } else {
                    return {};
                }
            } else {
                return {};
            }
        } else {
            return {{type, valueHolder}};
        }
    }

    std::tuple<const babelwires::Type&, babelwires::ValueHolder&>
    followNonConst(const babelwires::TypeSystem& typeSystem, const babelwires::Type& type,
                   babelwires::ValueHolder& valueHolder, const babelwires::Path& p, int& index) {
        if (index < p.getNumSteps()) {
            if (auto* compoundType = type.as<babelwires::CompoundType>()) {
                const int childIndex = compoundType->getChildIndexFromStep(valueHolder, p.getStep(index));
                if (childIndex >= 0) {
                    auto [childValue, _, childType] = compoundType->getChildNonConst(valueHolder, childIndex);
                    ++index;
                    return followNonConst(typeSystem, *childType, *childValue, p, index);
                } else {
                    throw babelwires::ModelException() << "No such child";
                }
            } else {
                throw babelwires::ModelException() << "Tried to step into a non-compound Value";
            }
        } else {
            return {type, valueHolder};
        }
    }

    void visitNonConst(const babelwires::TypeSystem& typeSystem, const babelwires::Type& type,
                       babelwires::ValueHolder& valueHolder, const std::span<const babelwires::Path*>& paths,
                       const std::function<void(const babelwires::Type&, babelwires::ValueHolder&)>& visitor,
                       unsigned int depth) {
        assert(paths.size() > 0);
        if ((paths.size() == 1) && (paths[0]->getNumSteps() == depth)) {
            visitor(type, valueHolder);
        } else if (const auto& compoundType = type.as<babelwires::CompoundType>()) {
            // Break the paths span into sub-spans, corresponding to children.
            auto left = paths.begin();
            auto next = left;
            do {
                ++next;
                const babelwires::PathStep currentStep = (*left)->getStep(depth);
                if ((next == paths.end()) || currentStep != (*next)->getStep(depth)) {
                    const std::span<const babelwires::Path*> childSpan(left, next);
                    const int childIndex = compoundType->getChildIndexFromStep(valueHolder, currentStep);
                    assert((childIndex >= 0) && "Path could not be followed");
                    auto [childValue, _, childType] = compoundType->getChildNonConst(valueHolder, childIndex);
                    visitNonConst(typeSystem, *childType, *childValue, childSpan, visitor, depth + 1);
                    left = next;
                }
            } while (left != paths.end());
        } else {
            assert(false && "Could not follow all paths");
        }
    }

} // namespace

std::optional<std::tuple<const babelwires::Type&, const babelwires::ValueHolder&>>
babelwires::tryFollowPath(const TypeSystem& typeSystem, const Type& type, const Path& path, const ValueHolder& start) {
    int index = 0;
    return tryFollow(typeSystem, type, start, path, index);
}

std::tuple<const babelwires::Type&, babelwires::ValueHolder&>
babelwires::followPathNonConst(const TypeSystem& typeSystem, const Type& type, const Path& path, ValueHolder& start) {
    int index = 0;
    try {
        return followNonConst(typeSystem, type, start, path, index);
    } catch (const std::exception& e) {
        throw babelwires::ModelException()
            << e.what() << "; when trying to follow step #" << index + 1 << " in path \"" << path << '\"';
    }
}

void babelwires::visitPathsNonConst(const TypeSystem& typeSystem, const Type& type, ValueHolder& value,
                                    const std::vector<Path>& paths,
                                    const std::function<void(const Type&, ValueHolder&)>& visitor) {
    std::vector<const Path*> sortedPathPointers;
    sortedPathPointers.reserve(paths.size());
    for (const Path& path : paths) {
        sortedPathPointers.emplace_back(&path);
    }
    std::sort(sortedPathPointers.begin(), sortedPathPointers.end());
    visitNonConst(typeSystem, type, value, sortedPathPointers, visitor, 0);
}
