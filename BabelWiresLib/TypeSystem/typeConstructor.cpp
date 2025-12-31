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

babelwires::TypePtr
babelwires::TypeConstructor::tryGetOrConstructType(const TypeSystem& typeSystem,
                                                   const TypeConstructorArguments& arguments) const {
    const auto& storage = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        babelwires::TypePtr operator()(std::monostate) {
            assert(false && "Attempt to construct a null type");
            return {};
        }
        babelwires::TypePtr operator()(const TypePtr& type) { return type; }
        babelwires::TypePtr operator()(const std::string& error) { return {}; }
    };
    return std::visit(VisitorMethods(), storage);
}

babelwires::TypePtr babelwires::TypeConstructor::getOrConstructType(const TypeSystem& typeSystem,
                                                                    const TypeConstructorArguments& arguments) const {
    const auto& storage = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        babelwires::TypePtr operator()(std::monostate) {
            assert(false && "Attempt to construct a null type");
            return {};
        }
        babelwires::TypePtr operator()(const TypePtr& type) { return type; }
        babelwires::TypePtr operator()(const std::string& error) { throw TypeSystemException() << error; }
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
    TypeRef newTypeRef(getTypeConstructorId(), arguments);

    {
        // Phase 3: Try the cache again with a write lock.
        // If it's still not found, construct and insert the new type.
        std::unique_lock lock(m_mutexForCache);

        auto it = m_cache.emplace(std::pair{arguments, PerTypeStorage()});
        if (it.second) {
            // Still not found.
            // Only construct the type if the arity is correct.
            if (resolvedArguments.size() == arguments.getTypeArguments().size()) {
                try {
                    TypePtr result = constructType(typeSystem, std::move(newTypeRef), arguments, resolvedArguments);
                    assert(result && "Returning a null pointer from a TypeConstructor is not permitted");
                    it.first->second = std::move(result);
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
