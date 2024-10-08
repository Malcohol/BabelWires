/**
 * A RecordFeature carries a sequence of features identified by FeatureIdentifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/compoundFeature.hpp>

namespace babelwires {

    /// Carries a sequence of features identified by FeatureIdentifier.
    /// The sequence of fields will usually be stable, although that is not a requirement.
    /// For example, the set of fields in the output feature of a processor can depend on the input.
    class RecordFeature : public CompoundFeature {
      public:
        /// The identifier must be already be registered with a field name.
        /// Fields are normally give names using the BW_SHORT_ID macro in fieldName.h.
        /// The identifier need only be unique in the Record.
        template <typename T> T* addField(std::unique_ptr<T> f, const ShortId& identifier);

        ShortId getFieldIdentifier(int index) const;

        virtual int getNumFeatures() const override;
        virtual PathStep getStepToChild(const Feature* child) const override;

        int getChildIndexFromStep(const PathStep& identifier) const override;

      protected:
        virtual void doSetToDefault() override;
        virtual void doSetToDefaultNonRecursive() override;
        virtual Feature* doGetFeature(int i) override;
        virtual const Feature* doGetFeature(int i) const override;
        virtual std::size_t doGetHash() const override;

        /// The per-field data.
        struct Field {
            ShortId m_identifier;
            std::unique_ptr<Feature> m_feature;
        };

        void addFieldInternal(Field field, int index = -1);

        struct FieldAndIndex : Field {
            int m_index = -1;
        };

        /// Convenience method which calls the addFieldInternal.
        void addFieldAndIndexInternal(FieldAndIndex fieldAndIndex);

        FieldAndIndex removeField(ShortId identifier);

      protected:
        std::vector<Field> m_fields;
    };

} // namespace babelwires

#include <BabelWiresLib/Features/recordFeature_inl.hpp>
