/**
 * Functions for exploring values with FeaturePaths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/valuePath.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace {

    std::tuple<const babelwires::Type&, babelwires::ValueHolder&> followPathNonConst(const babelwires::TypeSystem& typeSystem, const babelwires::Type& type, babelwires::ValueHolder& valueHolder, const babelwires::Path& p, int& index) {
        if (index < p.getNumSteps()) {
            if (auto* compoundType = type.as<babelwires::CompoundType>()) {
                const int childIndex = compoundType->getChildIndexFromStep(*valueHolder, p.getStep(index));
                if (childIndex >= 0) {
                    auto [childValue, _, childType] = compoundType->getChildNonConst(valueHolder, childIndex);
                    ++index;
                    return followPathNonConst(typeSystem, childType.resolve(typeSystem), *childValue, p, index);
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

} // namespace

std::tuple<const babelwires::Type&, babelwires::ValueHolder&> babelwires::followNonConst(const TypeSystem& typeSystem, const Type& type, const Path& path, ValueHolder& start) {
    int index = 0;
    try {
        return followPathNonConst(typeSystem, type, start, path, index);
    } catch (const std::exception& e) {
        throw babelwires::ModelException()
            << e.what() << "; when trying to follow step #" << index + 1 << " in path \"" << path << '\"';
    }
}
