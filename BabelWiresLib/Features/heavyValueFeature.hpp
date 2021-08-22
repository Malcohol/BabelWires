/**
 * A feature which carries an immutable, shareable value of T.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/features.hpp"

#include <memory>

namespace babelwires {

    /// Carries an immutable, shareable value of T.
    template <typename T> class HeavyValueFeature : public ValueFeature {
      public:
        /// Carries a default constructed T.
        HeavyValueFeature();

        /// Get the value held by the feature.
        const T& get() const;

        /// Move the value into the feature.
        void set(T&& newValue);

        /// Copy the value into the feature
        void set(const T& newValue);

        /// Take ownership of the value.
        void set(std::unique_ptr<T> newValue);

      protected:
        void doAssign(const ValueFeature& other) override;
        void doSetToDefault() override;
        std::size_t doGetHash() const override;

      protected:
        std::shared_ptr<const T> m_value;
    };

} // namespace babelwires

#include "BabelWiresLib/Features/heavyValueFeature_inl.hpp"
