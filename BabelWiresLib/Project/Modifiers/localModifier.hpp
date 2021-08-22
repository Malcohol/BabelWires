/**
 * A LocalModifier is a modifier which operates on the contents of a single FeatureElement, and is unaffected by context.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/Modifiers/modifier.hpp"

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct LocalModifierData;

    /// A modifier which operates on the contents of a single FeatureElement, and is unaffected by context.
    class LocalModifier : public Modifier {
      public:
        LocalModifier(std::unique_ptr<LocalModifierData> modifierData);
        LocalModifier(const LocalModifier& other);
        CLONEABLE(LocalModifier);

        LocalModifierData& getModifierData();
        const LocalModifierData& getModifierData() const;

        void applyIfLocal(UserLogger& userLogger, Feature* container) override;

      protected:
        const ConnectionModifier* doAsConnectionModifier() const override;
    };

} // namespace babelwires
