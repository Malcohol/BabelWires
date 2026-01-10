/**
 * A type which can stand in when a type or node fails to resolve.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/TypeSystem/registeredType.hpp>

namespace babelwires {

    /// A type which can stand in when a type or node fails to resolve.
    /// For now, just implement as an empty record.
    class FailureType : public RecordType {
      public:
        REGISTERED_TYPE("failed", "Failed", "d58040ff-00dc-4f25-a9a7-17c54b56d57d", 1);

        FailureType(const TypeSystem& typeSystem) : RecordType(getThisIdentifier(), typeSystem, {}) {}

        std::string getFlavour() const override { return "Failed"; }
    };

}
