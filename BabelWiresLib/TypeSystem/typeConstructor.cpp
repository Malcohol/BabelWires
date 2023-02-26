/**
 * A TypeRef identifies a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>

#include <mutex>

const babelwires::Type*
babelwires::TypeConstructor::getOrConstructType(const TypeSystem& typeSystem,
                                                const TypeConstructorArguments& arguments) const {
    if (getArity() != arguments.m_typeArguments.size()) {
        return nullptr;
    }

    {
        // Phase 1: Try cache read-only
        std::shared_lock lock(m_mutexForCache);

        auto it = m_cache.find(arguments);
        if (it != m_cache.end()) {
            return it->second.get();
        }
    }

    // Phase 2: Resolve the arguments.
    std::vector<const Type*> resolvedArguments;
    for (auto arg : arguments.m_typeArguments) {
        if (const Type* const argAsType = arg.tryResolve(typeSystem)) {
            resolvedArguments.emplace_back(argAsType);
        }
    }

    {
        // Phase 3: Try the cache again with a write lock.
        // If it's still not found, construct and insert the new type.
        std::unique_lock lock(m_mutexForCache);

        auto it = m_cache.emplace(std::pair{arguments, nullptr});
        if (it.second) {
            // Still not found.
            // Only construct the type if the 
            if (resolvedArguments.size() == arguments.m_typeArguments.size()) {
                it.first->second = constructType(resolvedArguments);
            }
        }
        return it.first->second.get();
    }
}
