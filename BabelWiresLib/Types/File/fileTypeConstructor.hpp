/**
 * A TypeConstructor which constructs a file type to wrap a particular value type for its contents.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    /// Construct a new IntType from three IntValues: min, max and default.
    class BABELWIRESLIB_API FileTypeConstructor : public TypeConstructor {
      public:
        DOWNCASTABLE(FileTypeConstructor, TypeConstructor);
        /// Note that the we don't represent the default in the name.
        TYPE_CONSTRUCTOR("File", "{{{0}}}", "a1fb6071-1a75-4568-b0fa-dce4e1d26610", 1);

        /// Convenience method.
        static TypeExp makeTypeExp(TypeExp typeOfContents);

        ResultT<TypePtr> constructType(const TypeSystem& typeSystem, TypeExp newTypeExp, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;
    };
} // namespace babelwires