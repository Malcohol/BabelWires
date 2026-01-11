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
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

namespace {
    /// Calculate how many generic type levels this type has.
    unsigned int calculateGenericTypeHeight(const babelwires::TypeExp& wrappedType) {
        struct Visitor {
            unsigned int operator()(std::monostate) { return 0; }
            unsigned int operator()(const babelwires::RegisteredTypeId& typeId) {
                // A type variable does not work if it's inside a registered type.
                return 0;
            }
            unsigned int operator()(const babelwires::TypeConstructorId& constructorId,
                                    const babelwires::TypeConstructorArguments& constructorArguments) {
                unsigned int height = 0;
                for (const auto& arg : constructorArguments.getTypeArguments()) {
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

babelwires::GenericType::GenericType(TypeExp&& typeExpOfThis, const TypeSystem& typeSystem, const TypeExp& wrappedType,
                                     unsigned int numVariables)
    : GenericType(std::move(typeExpOfThis), wrappedType.resolve(typeSystem), numVariables) {}

babelwires::GenericType::GenericType(TypeExp&& typeExpOfThis, const TypePtr& wrappedType, unsigned int numVariables)
    : CompoundType(std::move(typeExpOfThis))
    , m_wrappedType(std::move(wrappedType))
    , m_numVariables(numVariables) {
    assert(m_numVariables > 0 && "GenericType must have at least one type variable");
    assert(m_numVariables <= TypeVariableData::c_maxNumTypeVariables && "GenericType with too many type variables");
    // Height is used to choose a visualization of type variables.
    // Registered types that are themselves generic types could lead to duplication, but that's not a big problem.
    m_genericTypeHeight = calculateGenericTypeHeight(m_wrappedType->getTypeExp());
}

const babelwires::TypeExp& babelwires::GenericType::getTypeAssignment(const ValueHolder& genericValue,
                                                                      unsigned int variableIndex) const {
    const GenericValue& value = genericValue->as<GenericValue>();
    const auto& typeAssignments = value.getTypeAssignments();
    assert(variableIndex < typeAssignments.size() && "Variable index out of bounds");
    return typeAssignments[variableIndex];
}

bool babelwires::GenericType::isAnyTypeVariableUnassigned(const ValueHolder& genericValue) const {
    const GenericValue& value = genericValue->as<GenericValue>();
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

bool babelwires::GenericType::visitValue(const TypeSystem& typeSystem, const Value& v,
                                         ChildValueVisitor& visitor) const {
    const GenericValue* const genericValue = v.tryAs<GenericValue>();
    if (!genericValue) {
        return false;
    }
    if (m_numVariables != genericValue->getTypeAssignments().size()) {
        return false;
    }
    return genericValue->isActualVersionOf(m_wrappedType->getTypeExp());
}

unsigned int babelwires::GenericType::getNumChildren(const ValueHolder& compoundValue) const {
    static_assert(c_numChildren == 1, "c_numChildren must be 1");
    return c_numChildren;
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypePtr&>
babelwires::GenericType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    const GenericValue& genericValue = compoundValue->as<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getActualWrappedType()};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypePtr&>
babelwires::GenericType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    GenericValue& genericValue = compoundValue.copyContentsAndGetNonConst().as<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getActualWrappedType()};
}

int babelwires::GenericType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
    assert(step == getStepToValue() && "The step is not the expected step for GenericType");
    assert(compoundValue->as<GenericValue>().getValue() && "The value is not a GenericValue");
    return 0;
}

std::optional<babelwires::SubtypeOrder> babelwires::GenericType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                      const Type& other) const {
    const auto* otherGenericType = other.tryAs<GenericType>();
    if (!otherGenericType) {
        return SubtypeOrder::IsDisjoint;
    }
    if ((m_numVariables == otherGenericType->m_numVariables) && (m_wrappedType == otherGenericType->m_wrappedType)) {
        return SubtypeOrder::IsEquivalent;
    }
    return SubtypeOrder::IsDisjoint;
}

std::string babelwires::GenericType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const GenericValue& genericValue = v->as<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    auto typeAssignments = genericValue.getTypeAssignments();
    const char* sep = "";
    std::ostringstream os;
    os << "<";
    for (unsigned int i = 0; i < typeAssignments.size(); ++i) {
        const TypeExp& assignment = typeAssignments[i];
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
    const std::vector<TypeExp>& typeVariableAssignments) const {
    if (typeVariableAssignments.size() > m_numVariables) {
        throw ModelException() << "Too many type variable assignments for GenericType";
    }
    const GenericValue& constGenericValue = genericValue->as<GenericValue>();
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
    GenericValue& mutableGenericValue = genericValue.copyContentsAndGetNonConst().as<GenericValue>();
    mutableGenericValue.getTypeAssignments() = typeVariableAssignments;
    mutableGenericValue.instantiate(typeSystem, m_wrappedType->getTypeExp());
}
