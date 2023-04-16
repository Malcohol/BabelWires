/**
 * ValueFeatures carrying numeric values, e.g. Integers and Rationals.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <Common/Math/rational.hpp>

namespace babelwires {

    class ValueNames;

    template <typename T> class NumericFeature : public ValueFeature {
      public:
        T get() const;
        virtual void set(T value);

        /// Get the range of valid values.
        virtual Range<T> getRange() const;

      protected:
        /// By default, uses 0 or, if that isn't within the range, the minimum of the range.
        virtual void doSetToDefault() override;

        /// set(other.get())
        virtual void doAssign(const ValueFeature& other) override;

        /// Returns the std::hash of m_value.
        virtual std::size_t doGetHash() const override;

        typedef T ValueType;

      private:
        T m_value = 0;
    };

    // TODO Provide floating point features as standard.

} // namespace babelwires

#include <BabelWiresLib/Features/numericFeature_inl.hpp>
