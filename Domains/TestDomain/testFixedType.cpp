/**
 * TestFixedType
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Domains/TestDomain/testFixedType.hpp>

testDomain::TestFixedType::TestFixedType()
    : babelwires::FixedType(getThisIdentifier(), 2, babelwires::Range<babelwires::Fixed::NativeType>(-250, 425), 0) {}