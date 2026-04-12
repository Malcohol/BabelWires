/**
 * Register factories etc. for the TestDomain into BabelWires.
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Domains/TestDomain/testDomainExport.hpp>

namespace babelwires {
    class Context;
}

namespace testDomain {
    /// This is not a true plugin model, because everything is statically linked.
    TESTDOMAIN_API void registerLib(babelwires::Context& context);
} // namespace testDomain
