#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <cassert>

namespace testUtils {
    /// Returns a TestType with an extended maximum length
    class TestUnaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Unary"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::EditableValueHolder>& valueArguments) const override;
    };

    class TestBinaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Binary"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::EditableValueHolder>& valueArguments) const override;
    };

    /// Takes a TestType and a StringValue and constructs a new TestType with the value concatenated onto its default.
    class TestMixedTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Mixed"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::EditableValueHolder>& valueArguments) const override;
    };

} // namespace testUtils
