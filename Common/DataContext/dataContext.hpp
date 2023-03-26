/**
 * A DataContext is expected to provide some context specific data during high-level serialization
 * and deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    class DeserializationRegistry;

    /// A DataContext is expected to provide some context specific data during high-level serialization
    /// and deserialization.
    struct DataContext {
        DeserializationRegistry& m_deserializationReg;
    };
} // namespace babelwires
