/**
 * A type which can stand in when a type or element fails to resolve in the current system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    /// A type which can stand in when a type or element fails to resolve in the current system.
    class FailureType : public Type {
      public:
        /// The message is the reason for the failure.
        FailureType(std::string message);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        std::string m_message;
    };

}
