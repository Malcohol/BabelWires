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
    auto typeOrError = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        babelwires::TypePtr operator()(const TypePtr& type) { return type; }
        babelwires::TypePtr operator()(const std::string& error) { return {}; }
    };
    return std::visit(VisitorMethods(), typeOrError);
}

babelwires::TypePtr babelwires::TypeConstructor::getOrConstructType(const TypeSystem& typeSystem,
                                                                    const TypeConstructorArguments& arguments) const {
    auto typeOrError = getOrConstructTypeInternal(typeSystem, arguments);
    struct VisitorMethods {
        babelwires::TypePtr operator()(const TypePtr& type) { return type; }
        babelwires::TypePtr operator()(const std::string& error) { throw TypeSystemException() << error; }
    };
    return std::visit(VisitorMethods(), typeOrError);
}

babelwires::TypeConstructor::TypeOrError
babelwires::TypeConstructor::getOrConstructTypeInternal(const TypeSystem& typeSystem,
                                                        const TypeConstructorArguments& arguments) const {
    {
        // Phase 1: Try cache read-only
        std::shared_lock lock(m_mutexForCache);

        auto it = m_cache.find(arguments);
        if (it != m_cache.end()) {
            if (const WeakTypePtr* const weakPtr = std::get_if<WeakTypePtr>(&it->second)) {
                if (TypePtr owningPtr = weakPtr->lock()) {
                    return owningPtr;
                }
            } else {
                return std::get<std::string>(it->second);
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
            if (const WeakTypePtr* const weakPtr = std::get_if<WeakTypePtr>(&it.first->second)) {
                if (TypePtr owningPtr = weakPtr->lock()) {
                    return owningPtr;
                }
            } else {
                return std::get<std::string>(it.first->second);
            }
        }
        // Still not found.
        // Only construct the type if the arity is correct.
        if (resolvedArguments.size() == arguments.getTypeArguments().size()) {
            try {
                TypePtr result = constructType(typeSystem, std::move(newTypeExp), arguments, resolvedArguments);
                assert(result && "Returning a null pointer from a TypeConstructor is not permitted");
                it.first->second = result;
                return result;
            } catch (TypeSystemException& e) {
                std::string result = e.what();
                it.first->second = result;
                return result;
            }
        } else {
            std::ostringstream os;
            os << "Failed to construct a type because the following type arguments failed to resolve: ";
            const char* sep = "";
            for (auto refString : unresolvedTypesString) {
                os << sep << refString;
                sep = ", ";
            }
            std::string result = os.str();
            it.first->second = result;
            return result;
        }
    }
}

babelwires::TypePtr
babelwires::TypeConstructor::constructWithoutCaching(const TypeSystem& typeSystem, RegisteredTypeId typeId,
                                                     const TypeConstructorArguments& arguments) const {

    // TODO Unify with above and tidy up
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

    if (resolvedArguments.size() == arguments.getTypeArguments().size()) {
        TypeExp newTypeExp(typeId);
        return constructType(typeSystem, std::move(newTypeExp), arguments, resolvedArguments);
    } else {
        std::ostringstream os;
        os << "Failed to construct a type because the following type arguments failed to resolve: ";
        const char* sep = "";
        for (auto refString : unresolvedTypesString) {
            os << sep << refString;
            sep = ", ";
        }
        throw TypeSystemException() << os.str();
    }
}
