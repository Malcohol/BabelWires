/**
 * A TypeConstructor constructs a type from other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

template<unsigned int N>
const babelwires::Type*
babelwires::TypeConstructor<N>::getOrConstructType(const TypeSystem& typeSystem,
                                                const TypeConstructorArguments<N>& arguments) const {
    {
        // Phase 1: Try cache read-only
        std::shared_lock lock(m_mutexForCache);

        auto it = m_cache.find(arguments);
        if (it != m_cache.end()) {
            return it->second.get();
        }
    }

    // Phase 2: Resolve the arguments.
    std::array<const Type*, N> resolvedArguments;
    for (int i = 0; i < N; ++i) {
        const TypeRef& arg = arguments.m_typeArguments[i];
        if (const Type* const argAsType = arg.tryResolve(typeSystem)) {
            resolvedArguments[i] = argAsType;
        }
    }
    TypeRef newTypeRef(getTypeConstructorId(), arguments);

    {
        // Phase 3: Try the cache again with a write lock.
        // If it's still not found, construct and insert the new type.
        std::unique_lock lock(m_mutexForCache);

        auto it = m_cache.emplace(std::pair{arguments, nullptr});
        if (it.second) {
            // Still not found.
            // Only construct the type if the arity is correct.
            if (resolvedArguments.size() == arguments.m_typeArguments.size()) {
                it.first->second = constructType(std::move(newTypeRef), resolvedArguments);
            }
        }
        return it.first->second.get();
    }
}

template<unsigned int N>
babelwires::SubtypeOrder babelwires::TypeConstructor<N>::compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments<N>& arguments, const TypeRef& other) const {
    return SubtypeOrder::IsUnrelated;
}

template<unsigned int N>
babelwires::SubtypeOrder babelwires::TypeConstructor<N>::compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments<N>& argumentsA, const TypeConstructorArguments<N>& argumentsB) const {
    return SubtypeOrder::IsUnrelated;
}
