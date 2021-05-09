/**
 * A RecordFeature carries a sequence of features identified by FeatureIdentifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/features.hpp"

namespace babelwires {

    /// Carries a sequence of features identified by FeatureIdentifier.
    /// The sequence of fields will usually be fixed, although that is not a requirement.
    /// For example, the set of fields in the output feature of a processor can depend on the
    /// input.
    class RecordFeature : public CompoundFeature {
      public:
        /// The identifier must be already be registered with a field name.
        /// Fields are normally give names using the FIELD_NAME macro in fieldName.h.
        /// The identifier need only be unique in the Record.
        template <typename T> T* addField(std::unique_ptr<T> f, const FieldIdentifier& identifier);

        FieldIdentifier getFieldIdentifier(int index) const;

        virtual int getNumFeatures() const override;
        virtual PathStep getStepToChild(const Feature* child) const override;
        virtual Feature* tryGetChildFromStep(const PathStep& step) override;
        virtual const Feature* tryGetChildFromStep(const PathStep& step) const override;

        /// Returns -1 if not found.
        /// Sets the descriminator of identifier on a match.
        int getChildIndexFromStep(const FieldIdentifier& identifier) const;

      protected:
        virtual Feature* doGetFeature(int i) override;
        virtual const Feature* doGetFeature(int i) const override;
        virtual std::size_t doGetHash() const override;

        /// The per-field data.
        struct Field {
            FieldIdentifier m_identifier;
            std::unique_ptr<Feature> m_feature;
        };

        void addFieldInternal(Field field, int index = -1);

        struct FieldAndIndex : Field {
            int m_index = -1;
        };

        FieldAndIndex removeField(FieldIdentifier identifier);

      protected:
        std::vector<Field> m_fields;
    };

} // namespace babelwires

#include "BabelWires/Features/recordFeature_inl.hpp"
