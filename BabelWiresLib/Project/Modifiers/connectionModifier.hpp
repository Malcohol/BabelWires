/**
 * A ConnectionModifier connects features from two FeatureElements together.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct ConnectionModifierData;

    /// A ConnectionModifier connects features from two FeatureElements together.
    /// By contrast with LocalModifiers, it application depends on context.
    class ConnectionModifier : public Modifier {
      public:
        ConnectionModifier(std::unique_ptr<ConnectionModifierData> modifierData);
        ConnectionModifier(const ConnectionModifier& other);
        CLONEABLE(ConnectionModifier);

        ConnectionModifierData& getModifierData();
        const ConnectionModifierData& getModifierData() const;

        /// Usually this modifier only does work if the source is changed.
        /// However, if the modifier IS_NEW, then it is applied anyway.
        void applyConnection(const Project& project, UserLogger& userLogger, Feature* container, bool shouldForce = false);

        /// Convenience method: Did source and target resolve?
        bool isConnected() const;

        /// If the modifier targets an element in the array below the startIndex, apply the adjustment.
        void adjustSourceArrayIndices(const babelwires::FeaturePath& pathToArray, babelwires::ArrayIndex startIndex,
                                      int adjustment);

      protected:
        const ConnectionModifier* doAsConnectionModifier() const override;
    };

} // namespace babelwires
