/**
 * An interface for looking up deserialization information about classes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>
#include <BaseLib/common.hpp>
#include <functional>
#include <string_view>

namespace babelwires {

    class Deserializer;
    struct Serializable;

    struct BASELIB_API DeserializationTreeNode {
        /// Either the provided name for concrete classes using the SERIALIZABLE macro, or a generated string for abstract classes.
        std::string_view m_serializationType;
        const DeserializationTreeNode* m_parentNode = nullptr;
    };

    /// An interface for looking up deserialization information about classes.
    class BASELIB_API DeserializationRegistryInterface {
      public:
        using Factory = std::function<ResultT<std::unique_ptr<Serializable>>(Deserializer& deserializer)>;

        virtual ~DeserializationRegistryInterface() = default;

        /// Instances of this object represent the registration of a single concrete class' deserializingFactory.
        struct BASELIB_API Entry {
            Factory m_factory;
            VersionNumber m_version = 0;
            // The serializationTypeName of this type and all of its parents.
            const DeserializationTreeNode* m_node = nullptr;
        };

        /// Returns nullptr if an entry cannot be found.
        virtual const Entry* findEntry(std::string_view typeName) const = 0;
    };

} // namespace babelwires
