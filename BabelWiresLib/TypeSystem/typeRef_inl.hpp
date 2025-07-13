inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef typeRef0) 
    : TypeRef(typeConstructorId, TypeConstructorArguments{{std::move(typeRef0)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef typeRef0, TypeRef typeRef1)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{std::move(typeRef0), std::move(typeRef1)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, EditableValueHolder value0)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, EditableValueHolder value0, EditableValueHolder value1)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0), std::move(value1)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, EditableValueHolder value0, EditableValueHolder value1, EditableValueHolder value2)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0), std::move(value1), std::move(value2)}}) {}

template <typename Visitor, typename R>
R babelwires::TypeRef::visit(Visitor& visitor) const {
    struct VisitorMethods {
        R operator()(std::monostate) {
            return visitor(std::monostate());
        }
        R operator()(const RegisteredTypeId& typeId) { 
            return visitor(typeId);
        }
        R operator()(const ConstructedTypeData& constructedTypeData) {
            return visitor(std::get<0>(constructedTypeData), std::get<1>(constructedTypeData));
        }
        Visitor& visitor;
    } visitorMethods{visitor};
    return std::visit(visitorMethods, m_storage);
}
