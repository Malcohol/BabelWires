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
        const babelwires::Type* operator()(std::monostate) { assert(false && "Attempt to construct a null type"); return nullptr; }
        const babelwires::Type* operator()(const std::unique_ptr<Type>& type) { return type.get(); }
        const babelwires::Type* operator()(const Type* type) { return type; }
        const babelwires::Type* operator()(const std::string& error) { return nullptr; }
    };
    return std::visit(VisitorMethods(), storage);
}

const babelwires::Type&
babelwires::TypeConstructor::getOrConstructType(const TypeSystem& typeSystem,
                                                const TypeConstructorArguments& arguments) const {
    const auto& storage = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        const babelwires::Type& operator()(std::monostate) { assert(false && "Attempt to construct a null type"); return *(const babelwires::Type*)0; }
        const babelwires::Type& operator()(const std::unique_ptr<Type>& type) { return *type; }
        const babelwires::Type& operator()(const Type* type) { return *type; }
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
    resolvedArguments.reserve(arguments.m_typeArguments.size());
    std::vector<std::string> unresolvedTypesString;
    for (auto arg : arguments.m_typeArguments) {
        if (const Type* const argAsType = arg.tryResolve(typeSystem)) {
            resolvedArguments.emplace_back(argAsType);
        } else {
            unresolvedTypesString.emplace_back(arg.toString());
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
                    TypeConstructorResult result = constructType(typeSystem, std::move(newTypeRef), resolvedArguments,
                                                                 arguments.m_valueArguments);
                    if (std::holds_alternative<std::unique_ptr<Type>>(result)) {
                        assert(std::get<std::unique_ptr<Type>>(result) &&
                            "Returning a null unique pointer from a TypeConstructor is not permitted");
                        it.first->second = std::move(std::get<std::unique_ptr<Type>>(result));
                    } else {
                        assert(std::get<const Type*>(result) &&
                            "Returning a null const pointer from a TypeConstructor is not permitted");
                        it.first->second = std::get<const Type*>(result);
                    }
                } catch (TypeSystemException& e) {
                    it.first->second = e.what();
                }
            } else {
                std::ostringstream os;
                os << "Failed to construct a type because the following type arguments failed to resolve: ";
                const char* sep = "";
                for (auto refString : unresolvedTypesString) {
                    os << sep << refString;
                    sep = ", ";
                }
                it.first->second = os.str();
            }
        }
        return it.first->second;
    }
}
