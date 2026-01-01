inline babelwires::TypeConstructorArguments::TypeConstructorArguments(std::vector<TypeExp> typeArguments)
    : m_typeArguments(std::move(typeArguments)) {}

inline babelwires::TypeConstructorArguments::TypeConstructorArguments(std::vector<TypeExp> typeArguments, std::vector<ValueHolder> valueArguments)
    : m_typeArguments(std::move(typeArguments))
    , m_valueArguments(std::move(valueArguments)) {
#ifndef NDEBUG
    for (const auto& v : valueArguments) {
        assert(v->tryGetAsEditableValue() && "Values stored by TypeConstructors must be editable");
    }
#endif
}

inline babelwires::TypeConstructorArguments::~TypeConstructorArguments() = default;

inline babelwires::TypeExp::TypeExp(TypeConstructorId typeConstructorId, TypeExp typeRef0) 
    : TypeExp(typeConstructorId, TypeConstructorArguments{{std::move(typeRef0)}}) {}

inline babelwires::TypeExp::TypeExp(TypeConstructorId typeConstructorId, TypeExp typeRef0, TypeExp typeRef1)
    : TypeExp(typeConstructorId, TypeConstructorArguments{{std::move(typeRef0), std::move(typeRef1)}}) {}

inline babelwires::TypeExp::TypeExp(TypeConstructorId typeConstructorId, ValueHolder value0)
    : TypeExp(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0)}}) {}

inline babelwires::TypeExp::TypeExp(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1)
    : TypeExp(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0), std::move(value1)}}) {}

inline babelwires::TypeExp::TypeExp(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1, ValueHolder value2)
    : TypeExp(typeConstructorId, TypeConstructorArguments{{}, {std::move(value0), std::move(value1), std::move(value2)}}) {}

template <typename Visitor, typename R>
R babelwires::TypeExp::visit(Visitor& visitor) const {
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

template <typename Visitor, typename R>
R babelwires::TypeExp::visit(Visitor& visitor, const TypeExp& a, const TypeExp& b) {
    struct VisitorMethods {
        R operator()(std::monostate, std::monostate) { return visitor(std::monostate(), std::monostate()); }
        R operator()(const RegisteredTypeId& typeIdA, const RegisteredTypeId& typeIdB) {
            return visitor(typeIdA, typeIdB);
        }
        R operator()(const ConstructedTypeData& constructedTypeDataA,
                        const ConstructedTypeData& constructedTypeDataB) {
            return visitor(std::get<0>(constructedTypeDataA), std::get<1>(constructedTypeDataA),
                            std::get<0>(constructedTypeDataB), std::get<1>(constructedTypeDataB));
        }
        R operator()(...) {
            return visitor();
        }
        Visitor& visitor;
    } visitorMethods{visitor};
    return std::visit(visitorMethods, a.m_storage, b.m_storage);
}