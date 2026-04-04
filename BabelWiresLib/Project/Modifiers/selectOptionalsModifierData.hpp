/**
 * SelectOptionalsModifierData is used to select a set of optionals in a RecordType
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

#include <map>

namespace babelwires {
    /// SelectOptionalsModifierData is used to select a set of optionals in a RecordType
    class BABELWIRESLIB_API SelectOptionalsModifierData : public LocalModifierData {
      public:
        DOWNCASTABLE(SelectOptionalsModifierData, LocalModifierData);
        virtual Result apply(ValueTreeNode* target) const override;
        CLONEABLE(SelectOptionalsModifierData);
        SERIALIZABLE(SelectOptionalsModifierData, "selectOptionals", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        Result deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// Select or deselect the given optional.
        /// isPositiveSelection means the optional is added to the selection.
        /// Otherwise it is removed from the selection.
        void setOptionalActivation(ShortId optional, bool isActivate);

        void resetOptionalActivation(ShortId optional);

        const std::map<ShortId, bool>& getOptionalActivationData() const;

        bool isDefaultState() const;

      private:
        std::map<ShortId, bool> m_optionalsActivation;
    };
}
