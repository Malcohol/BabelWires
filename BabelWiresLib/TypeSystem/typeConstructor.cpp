/**
 * A TypeConstructor constructs a type from other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>

#include <mutex>

babelwires::TypeConstructor::~TypeConstructor() = default;

const babelwires::Type*
babelwires::TypeConstructor::tryGetOrConstructType(const TypeSystem& typeSystem,
                                                   const TypeConstructorArguments& arguments) const {
    const auto& storage = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        const babelwires::Type* operator()(std::monostate) { assert(false); }
        const babelwires::Type* operator()(const std::unique_ptr<Type>& type) { return type.get(); }
        const babelwires::Type* operator()(const std::string& error) { return nullptr; }
    };
    return std::visit(VisitorMethods(), storage);
}

const babelwires::Type&
babelwires::TypeConstructor::getOrConstructType(const TypeSystem& typeSystem,
                                                const TypeConstructorArguments& arguments) const {
    const auto& storage = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        const babelwires::Type& operator()(std::monostate) { assert(false); }
        const babelwires::Type& operator()(const std::unique_ptr<Type>& type) { return *type; }
        const babelwires::Type& operator()(const std::string& error) { throw TypeSystemException() << error; }
    };
    return std::visit(VisitorMethods(), storage);
}

const babelwires::TypeConstructor::PerTypeStorage&
babelwires::TypeConstructor::getOrConstructTypeInternal(const TypeSystem& typeSystem,
                                                        const TypeConstructorArguments& arguments) const {
    {
        // Phase 1: Try cache read-only
        std::shared_lock lock(m_mutexForCache);

        auto it = m_cache.find(arguments);
        if (it != m_cache.end()) {
            return it->second;
        }
    }

    // Phase 2: Resolve the arguments.
    std::vector<const Type*> resolvedArguments;
    for (auto arg : arguments.m_typeArguments) {
        if (const Type* const argAsType = arg.tryResolve(typeSystem)) {
            resolvedArguments.emplace_back(argAsType);
        }
    }
    TypeRef newTypeRef(getTypeConstructorId(), arguments);

    {
        // Phase 3: Try the cache again with a write lock.
        // If it's still not found, construct and insert the new type.
        std::unique_lock lock(m_mutexForCache);

        auto it = m_cache.emplace(std::pair{arguments, PerTypeStorage()});
        if (it.second) {
            // Still not found.
            // Only construct the type if the arity is correct.
            if (resolvedArguments.size() == arguments.m_typeArguments.size()) {
                try {
                    it.first->second =
                        constructType(typeSystem, std::move(newTypeRef), resolvedArguments, arguments.m_valueArguments);
                    assert(std::get<std::unique_ptr<Type>>(it.first->second) &&
                           "Returning a null pointer from a TypeConstructor is not permitted");
                } catch (TypeSystemException& e) {
                    it.first->second = e.what();
                }
            } else {
                it.first->second = "Failed to construct a type because a type argument failed to resolve";
            }
        }
        return it.first->second;
    }
}
