inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef typeRef0) 
    : TypeRef(typeConstructorId, TypeConstructorArguments{{std::move(typeRef0)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef typeRef0, TypeRef typeRef1)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{std::move(typeRef0), std::move(typeRef1)}}) {}

/*
inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, ValueHolder value0)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0), std::move(value1)}}) {}

inline babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1, ValueHolder value2)
    : TypeRef(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0), std::move(value1), std::move(value2)}}) {}
*/