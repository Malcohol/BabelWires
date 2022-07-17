/**
 * ArraySizeModifier extends local modifier with some array specific methods.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>

namespace babelwires {

    struct ArraySizeModifierData;

    /// Extends local modifier with some array specific methods.
    // TODO This doesn't override any methods, so there's a bit of a code smell here.
    class ArraySizeModifier : public LocalModifier {
      public:
        ArraySizeModifier(std::unique_ptr<ArraySizeModifierData> modifierData);
        ArraySizeModifier(const ArraySizeModifier& other);
        CLONEABLE(ArraySizeModifier);

        ArraySizeModifierData& getModifierData();
        const ArraySizeModifierData& getModifierData() const;

        bool addArrayEntries(UserLogger& userLogger, Feature* container, int indexOfNewElement, int numEntriesToAdd);

        bool removeArrayEntries(UserLogger& userLogger, Feature* container, int indexOfElementToRemove,
                                int numEntriesToRemove);
    };

} // namespace babelwires
