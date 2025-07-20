/**
 * A TypeConstructor which constructs a file type to wrap a particular value type for its contents.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    /// Construct a new IntType from three IntValues: min, max and default.
    class FileTypeConstructor : public TypeConstructor {
      public:
        /// Note that the we don't represent the default in the name.
        TYPE_CONSTRUCTOR("File", "{{{0}}}", "a1fb6071-1a75-4568-b0fa-dce4e1d26610", 1);

        /// Convenience method.
        static TypeRef makeTypeRef(TypeRef typeOfContents);

        TypeConstructorResult constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;
    };
} // namespace babelwires