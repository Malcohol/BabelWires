/**
 * A ValueTreeRoot is a ValueTreeNode that owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;
    class Path;

    /// A ValueTreeRoot is a ValueTreeNode that owns its value.
    class ValueTreeRoot : public ValueTreeNode {
      public:
        /// Construct a rooted ValueTreeNode which carries values of the given type.
        ValueTreeRoot(const TypeSystem& typeSystem, TypeExp typeExp);

        /// Set the value at the path to the new value.
        void setDescendentValue(const Path& path, const ValueHolder& newValue);

        /// Get the TypeSystem carried by this root.
        const TypeSystem& getTypeSystem() const;

      protected:
        void doSetToDefault() override;
        void doSetValue(const ValueHolder& newValue) override;

      private:
        struct ComplexConstructorArguments;
        ValueTreeRoot(ComplexConstructorArguments&& arguments);

        /// Roots carry a reference to the typesystem.
        const TypeSystem& m_typeSystem;
    };

} // namespace babelwires
