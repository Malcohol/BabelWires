/**
 * Used by MapApplicators to convert abstract values to native values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

namespace babelwires {
    /// Used by MapApplicators to convert abstract values to native values.
    template <typename T> using ValueAdapter = std::function<T(const Value&)>;

} // namespace babelwires
