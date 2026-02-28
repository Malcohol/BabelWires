/**
 * A TypeConstructor constructs a type from other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>

#include <BaseLib/Result/error.hpp>

#include <mutex>

babelwires::TypeConstructor::~TypeConstructor() = default;

babelwires::TypePtr
babelwires::TypeConstructor::tryGetOrConstructType(const TypeSystem& typeSystem,
                                                   const TypeConstructorArguments& arguments) const {
    const auto typeOrError = getOrConstructType(typeSystem, arguments);
    if (typeOrError) {
        return *typeOrError;
    }
    return {};
}

babelwires::ResultT<babelwires::TypePtr>
babelwires::TypeConstructor::getOrConstructType(const TypeSystem& typeSystem,
                                                        const TypeConstructorArguments& arguments) const {
    {
        // Phase 1: Try cache read-only
        std::shared_lock lock(m_mutexForCache);

        auto it = m_cache.find(arguments);
        if (it != m_cache.end()) {
            if (it->second) {
                if (TypePtr owningPtr = it->second->lock()) {
                    return owningPtr;
                }
            } else {
                return std::unexpected(it->second.error());
            }
        }
    }

    // Phase 2: Resolve the arguments.
    std::vector<TypePtr> resolvedArguments;
    resolvedArguments.reserve(arguments.getTypeArguments().size());
    std::vector<std::string> unresolvedTypesString;
    for (auto arg : arguments.getTypeArguments()) {
        if (const TypePtr argAsType = arg.tryResolve(typeSystem)) {
            resolvedArguments.emplace_back(argAsType);
        } else {
            unresolvedTypesString.emplace_back(arg.toString());
        }
    }
    TypeExp newTypeExp(getTypeConstructorId(), arguments);

    {
        // Phase 3: Try the cache again with a write lock.
        // If it's still not found, construct and insert the new type.
        std::unique_lock lock(m_mutexForCache);

        auto it = m_cache.emplace(std::pair{arguments, PerTypeStorage()});
        if (!it.second) {
            // There's an entry now. See if it's usable.
            if (it.first->second) {
                if (TypePtr owningPtr = it.first->second->lock()) {
                    return owningPtr;
                }
            } else {
                return it.first->second.error();
            }
        }
        // Still not found.
        // Only construct the type if the arity is correct.
        if (resolvedArguments.size() == arguments.getTypeArguments().size()) {
            auto result = constructType(typeSystem, std::move(newTypeExp), arguments, resolvedArguments);
            if (result) {
                assert(*result && "Returning a null pointer from a TypeConstructor is not permitted");
                it.first->second = *result;
            } else {
                it.first->second = std::unexpected(result.error());
            }
            return result;
        } else {
            Error error;
            error << "Failed to construct a type because the following type arguments failed to resolve: ";
            const char* sep = "";
            for (auto refString : unresolvedTypesString) {
                error << sep << refString;
                sep = ", ";
            }
            it.first->second = error;
            return error;
        }
    }
}
