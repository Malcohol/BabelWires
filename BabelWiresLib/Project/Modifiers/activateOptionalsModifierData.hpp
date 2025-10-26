/**
 * ActivateOptionalsModifierData is used to select a set of optionals in a RecordType
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

#include <map>
namespace babelwires {
    /// ActivateOptionalsModifierData is used to select a set of optionals in a RecordType
    class ActivateOptionalsModifierData : public LocalModifierData {
      public:
        virtual void apply(ValueTreeNode* target) const override;
        CLONEABLE(ActivateOptionalsModifierData);
        SERIALIZABLE(ActivateOptionalsModifierData, "selectOptionals", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// Select or deselect the given optional.
        /// isPositiveSelection means the optional is added to the selection.
        /// Otherwise it is removed from the selection.
        void setOptionalActivation(ShortId optional, bool isActivate);

        void resetOptionalActivation(ShortId optional);

        std::map<ShortId, bool> getOptionalActivationData() const;

        bool isDefaultState() const;

      private:
        // TODO Should be a map
        std::vector<ShortId> m_activatedOptionals;
        std::vector<ShortId> m_deactivatedOptionals;
    };
}
