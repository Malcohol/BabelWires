#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <cassert>

namespace testUtils {
    /// Like TestType but it returns whatever typeRef it's told to.
    class TestConstructedType : public TestType {
      public:
        /// Takes the typeRef to return.
        TestConstructedType(babelwires::TypeRef thisTypeRef)
            : m_thisTypeRef(thisTypeRef) {}

        babelwires::TypeRef getTypeRef() const override { return m_thisTypeRef; }

        babelwires::TypeRef m_thisTypeRef;
    };

    /// Always returns TestConstructedType.
    class TestUnaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredLongIdentifier("Unary"), 1);

        unsigned int getArity() const override { return 1; }

        std::unique_ptr<babelwires::Type>
        constructType(const std::vector<const babelwires::Type*>& arguments) const override {
            assert(arguments.size() == 1);
            // Remember the typeRef, since there's no way to reconstruct it.
            return std::make_unique<TestConstructedType>(
                babelwires::TypeRef(getThisIdentifier(), {{arguments[0]->getTypeRef()}}));
        }
    };
} // namespace testUtils
