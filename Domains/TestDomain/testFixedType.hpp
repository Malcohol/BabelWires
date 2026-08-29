/**
 * TestFixedType
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Domains/TestDomain/testDomainExport.hpp>

#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Fixed/fixedType.hpp>

namespace testDomain {

    class TESTDOMAIN_API TestFixedType : public babelwires::FixedType {
      public:
        DOWNCASTABLE(TestFixedType, babelwires::FixedType);

        TestFixedType();

        REGISTERED_TYPE("testFixed", "Test Fixed", "0ce8968c-7055-4f8d-b98a-833035770ca7", 1);
    };

} // namespace testDomain