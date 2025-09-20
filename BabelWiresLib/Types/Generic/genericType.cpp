/**
 * GenericType is a type that wraps another type containing type variables that can be instantiated in different ways.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/genericValue.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

namespace {
    /// Calculate how many generic type levels this type has.
    unsigned int calculateGenericTypeHeight(const babelwires::TypeRef& wrappedType) {
        struct Visitor {
            unsigned int operator()(std::monostate) { return 0; }
            unsigned int operator()(const babelwires::RegisteredTypeId& typeId) {
                // A type variable does not work if it's inside a registered type.
                return 0;
            }
            unsigned int operator()(const babelwires::TypeConstructorId& constructorId,
                                    const babelwires::TypeConstructorArguments& constructorArguments) {
                unsigned int height = 0;
                for (const auto& arg : constructorArguments.m_typeArguments) {
                    height = std::max(height, calculateGenericTypeHeight(arg));
                }
                if (constructorId == babelwires::GenericTypeConstructor::getThisIdentifier()) {
                    // If this is a GenericType, we increment the height for its type arguments.
                    ++height;
                }
                return height;
            }
        } visitor;
        return wrappedType.visit<Visitor, unsigned int>(visitor);
    }
} // namespace

babelwires::GenericType::GenericType(TypeRef wrappedType, unsigned int numVariables)
    : m_wrappedType(std::move(wrappedType))
    , m_numVariables(numVariables) {
    assert(m_numVariables > 0 && "GenericType must have at least one type variable");
    assert(m_numVariables <= TypeVariableData::c_maxNumTypeVariables && "GenericType with too many type variables");
    // Height is used to choose a visualization of type variables.
    // Registered types that are themselves generic types could lead to duplication, but that's not a big problem.
    m_genericTypeHeight = calculateGenericTypeHeight(m_wrappedType);
}

const babelwires::TypeRef& babelwires::GenericType::getTypeAssignment(const ValueHolder& genericValue,
                                                                      unsigned int variableIndex) const {
    const GenericValue& value = genericValue->is<GenericValue>();
    const auto& typeAssignments = value.getTypeAssignments();
    assert(variableIndex < typeAssignments.size() && "Variable index out of bounds");
    return typeAssignments[variableIndex];
}

bool babelwires::GenericType::isAnyTypeVariableUnassigned(const ValueHolder& genericValue) const {
    const GenericValue& value = genericValue->is<GenericValue>();
    const auto& typeAssignments = value.getTypeAssignments();
    for (const auto& assignment : typeAssignments) {
        if (!assignment) {
            return true;
        }
    }
    return false;
}

unsigned int babelwires::GenericType::getNumVariables() const {
    return m_numVariables;
}

std::string babelwires::GenericType::getFlavour() const {
    return "Generic";
}

babelwires::NewValueHolder babelwires::GenericType::createValue(const TypeSystem& typeSystem) const {
    return babelwires::ValueHolder::makeValue<GenericValue>(typeSystem, m_wrappedType, m_numVariables);
}

bool babelwires::GenericType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const GenericValue* const genericValue = v.as<GenericValue>();
    if (!genericValue) {
        return false;
    }

    const std::vector<TypeRef>& typeAssignments = genericValue->getTypeAssignments();

    if (typeAssignments.size() != m_numVariables) {
        return false;
    }
    const auto actualWrappedType = genericValue->getActualWrappedType();

    struct Visitor {
        Visitor(const std::vector<TypeRef>& typeAssignments, unsigned int level = 0)
            : m_typeAssignments(typeAssignments)
            , m_level(level) {}
        bool operator()() { return false; } // Called when structurally different
        bool operator()(std::monostate, std::monostate) { return true; }
        bool operator()(const RegisteredTypeId& typeIdGen, const RegisteredTypeId& typeIdAct) {
            // Simplifying restriction for now: registered types may not contain unbound type variables.
            return typeIdGen == typeIdAct;
        }
        bool operator()(const TypeConstructorId& constructorIdRef,
                        const TypeConstructorArguments& constructorArgumentsRef,
                        const TypeConstructorId& constructorIdAct,
                        const TypeConstructorArguments& constructorArgumentsAct) {
            if (constructorIdRef != constructorIdAct) {
                return false;
            }
            if (constructorIdRef == TypeVariableTypeConstructor::getThisIdentifier()) {
                const TypeVariableData variableDataRef =
                    TypeVariableTypeConstructor::extractValueArguments(constructorArgumentsRef.m_valueArguments);
                const TypeVariableData variableDataAct =
                    TypeVariableTypeConstructor::extractValueArguments(constructorArgumentsAct.m_valueArguments);
                if ((variableDataRef.m_typeVariableIndex != variableDataAct.m_typeVariableIndex) ||
                    (variableDataRef.m_numGenericTypeLevels != variableDataAct.m_numGenericTypeLevels)) {
                    return false;
                }
                if (variableDataAct.m_numGenericTypeLevels == m_level) {
                    assert(variableDataAct.m_typeVariableIndex <= m_typeAssignments.size());
                    assert(!m_typeAssignments[variableDataAct.m_typeVariableIndex] ||
                           (constructorArgumentsAct.m_typeArguments.size() == 1));
                    assert(!m_typeAssignments[variableDataAct.m_typeVariableIndex] || 
                        (m_typeAssignments[variableDataAct.m_typeVariableIndex] ==
                           constructorArgumentsAct.m_typeArguments[0]));
                    // Allowed to differ here.
                    return true;
                }
            }
            unsigned int level = m_level;
            if (constructorIdRef == GenericTypeConstructor::getThisIdentifier()) {
                // Variables under this constructor would have to jump over this GenericType to
                // reach the one were processing.
                ++level;
            }
            // Compare the type and value arguments.
            if ((constructorArgumentsRef.m_typeArguments.size() != constructorArgumentsAct.m_typeArguments.size()) ||
                (constructorArgumentsRef.m_valueArguments.size() != constructorArgumentsAct.m_valueArguments.size())) {
                return false;
            }
            for (unsigned int i = 0; i < constructorArgumentsAct.m_valueArguments.size(); ++i) {
                if (constructorArgumentsRef.m_valueArguments[i] != constructorArgumentsAct.m_valueArguments[i]) {
                    return false;
                }
            }
            for (unsigned int i = 0; i < constructorArgumentsAct.m_typeArguments.size(); ++i) {
                Visitor childVisitor(m_typeAssignments, level);
                if (!TypeRef::visit<Visitor, bool>(childVisitor, constructorArgumentsRef.m_typeArguments[i],
                                                      constructorArgumentsAct.m_typeArguments[i])) {
                    return false;
                }
            }
            return true;
        }
        const std::vector<TypeRef>& m_typeAssignments;
        unsigned int m_level;
    } visitor(typeAssignments);
    return TypeRef::visit<Visitor, bool>(visitor, m_wrappedType, actualWrappedType);
}

unsigned int babelwires::GenericType::getNumChildren(const ValueHolder& compoundValue) const {
    static_assert(c_numChildren == 1, "c_numChildren must be 1");
    return c_numChildren;
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    const GenericValue& genericValue = compoundValue->is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getActualWrappedType()};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    GenericValue& genericValue = compoundValue.copyContentsAndGetNonConst().is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getActualWrappedType()};
}

int babelwires::GenericType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
    assert(step == getStepToValue() && "The step is not the expected step for GenericType");
    assert(compoundValue->is<GenericValue>().getValue() && "The value is not a GenericValue");
    return 0;
}

std::optional<babelwires::SubtypeOrder> babelwires::GenericType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                      const Type& other) const {
    // TODO
    return SubtypeOrder::IsDisjoint;
}

std::string babelwires::GenericType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const GenericValue& genericValue = v->is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    auto typeAssignments = genericValue.getTypeAssignments();
    const char* sep = "";
    std::ostringstream os;
    os << "<";
    for (unsigned int i = 0; i < typeAssignments.size(); ++i) {
        const TypeRef& assignment = typeAssignments[i];
        os << sep;
        if (assignment) {
            os << assignment.toString();
        } else {
            os << TypeVariableData{i}.toString();
        }
        sep = ", ";
    }
    os << ">";
    return os.str();
}

babelwires::ShortId babelwires::GenericType::getStepToValue() {
    return BW_SHORT_ID("wrappd", "generic value", "69d92618-a000-476e-afc1-9121e1bfac1e");
}

void babelwires::GenericType::setTypeVariableAssignmentAndInstantiate(
    const TypeSystem& typeSystem, ValueHolder& genericValue,
    const std::vector<TypeRef>& typeVariableAssignments) const {
    if (typeVariableAssignments.size() > m_numVariables) {
        throw ModelException() << "Too many type variable assignments for GenericType";
    }
    const GenericValue& constGenericValue = genericValue->is<GenericValue>();
    const auto& currentAssignments = constGenericValue.getTypeAssignments();

    bool changed = false;
    for (unsigned int i = 0; i < typeVariableAssignments.size(); ++i) {
        if (currentAssignments[i] != typeVariableAssignments[i]) {
            changed = true;
            break;
        }
    }
    if (!changed) {
        // No change, nothing to do.
        return;
    }
    GenericValue& mutableGenericValue = genericValue.copyContentsAndGetNonConst().is<GenericValue>();
    mutableGenericValue.getTypeAssignments() = typeVariableAssignments;
    mutableGenericValue.instantiate(typeSystem, m_wrappedType);
}
