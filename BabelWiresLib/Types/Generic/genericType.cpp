/**
 *
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

unsigned int babelwires::GenericType::getNumVariables() const {
    return m_numVariables;
}

std::string babelwires::GenericType::getFlavour() const {
    // Not connectable.
    return "";
}

babelwires::NewValueHolder babelwires::GenericType::createValue(const TypeSystem& typeSystem) const {
    return babelwires::ValueHolder::makeValue<GenericValue>(typeSystem, m_wrappedType, m_numVariables);
}

bool babelwires::GenericType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const GenericValue* genericValue = v.as<GenericValue>();
    if (!genericValue) {
        return false;
    }

    // TODO
    return true;
}

unsigned int babelwires::GenericType::getNumChildren(const ValueHolder& compoundValue) const {
    return 1;
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    const GenericValue& genericValue = compoundValue->is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getWrappedType()};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    GenericValue& genericValue = compoundValue.copyContentsAndGetNonConst().is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getWrappedType()};
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
    return BW_SHORT_ID("wrappd", "value", "69d92618-a000-476e-afc1-9121e1bfac1e");
}

void babelwires::GenericType::setTypeVariableAssignmentAndInstantiate(
    const TypeSystem& typeSystem, ValueHolder& genericValue, const std::vector<TypeRef>& typeVariableAssignments) const {
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
