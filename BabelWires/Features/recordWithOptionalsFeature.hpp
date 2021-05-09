/**
 * A RecordWithOptionalsFeature is a Record feature with subfeatures which can be inactive.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/recordFeature.hpp"

namespace babelwires {

    /// A Record feature with subfeatures which can be inactive.
    /// By default, an optional field is inactive.
    class RecordWithOptionalsFeature : public RecordFeature {
        public:
            /// Add an optional field, which is not present in the record until activated.
            template <typename T> T* addOptionalField(std::unique_ptr<T> f, const FieldIdentifier& identifier);

            /// Active the field, so it appears in the record.
            void activateField(FieldIdentifier identifier);

            /// Deactivate the field, so it does not appear in the record.
            /// This operation sets the subfeature to its default state.
            void deactivateField(FieldIdentifier identifier);

        protected:
            void addOptionalFieldInternal(FieldAndIndex f);

        protected:
            /// Those fields which are optional.
            std::vector<FieldIdentifier> m_optionalFields;

            /// The inactive fields, sorted by activeIndex;
            std::vector<FieldAndIndex> m_inactiveFields;
    };

    template <typename T> T* babelwires::RecordWithOptionalsFeature::addOptionalField(std::unique_ptr<T> f, const FieldIdentifier& identifier) {
        T* fTPtr = f.get();
        addOptionalFieldInternal(FieldAndIndex{identifier, std::move(f), getNumFeatures()});
        return fTPtr;
    }
} // namespace babelwires
