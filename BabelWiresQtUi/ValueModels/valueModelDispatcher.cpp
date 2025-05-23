/**
 * Dispatch to the appropriate value model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/valueModelDispatcher.hpp>

#include <BabelWiresQtUi/ValueModels/arrayValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/enumValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/intValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/mapValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/rationalValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/recordValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/recordWithVariantsValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/stringValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/sumValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/tupleValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/valueModelRegistry.hpp>

#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Map/SumOfMaps/sumOfMapsType.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

void babelwires::ValueModelDispatcher::init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem,
                                            const Type& type, const ValueHolder& value, bool isReadOnly,
                                            bool isStructureEditable) {
    m_valueModel = &m_valueModelStorage;
    // TODO Allow the UI to register callbacks into the corresponding ValueTypes.
    if (valueModelRegistry.handleValueTreeNodeOfType(&type, m_valueModel)) {
        // Handled by a registered handler.
    } else if (type.as<EnumType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::EnumValueModel));
        new (m_valueModel) babelwires::EnumValueModel();
    } else if (type.as<IntType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::IntValueModel));
        new (m_valueModel) babelwires::IntValueModel();
    } else if (type.as<RationalType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::RationalValueModel));
        new (m_valueModel) babelwires::RationalValueModel();
    } else if (type.as<StringType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::StringValueModel));
        new (m_valueModel) babelwires::StringValueModel();
    } else if (type.as<MapType>() || type.as<SumOfMapsType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::MapValueModel));
        new (m_valueModel) babelwires::MapValueModel();
    } else if (type.as<ArrayType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::ArrayValueModel));
        new (m_valueModel) babelwires::ArrayValueModel();
    } else if (type.as<RecordType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::RecordValueModel));
        new (m_valueModel) babelwires::RecordValueModel();
    } else if (type.as<RecordWithVariantsType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::RecordWithVariantsValueModel));
        new (m_valueModel) babelwires::RecordWithVariantsValueModel();
    } else if (type.as<TupleType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::TupleValueModel));
        new (m_valueModel) babelwires::TupleValueModel();
    } else if (type.as<SumType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::SumValueModel));
        new (m_valueModel) babelwires::SumValueModel();
    } else {
        // The base row model is used.
    }
    m_valueModel->m_typeSystem = &typeSystem;
    m_valueModel->m_type = &type;
    m_valueModel->m_value = &value;
    m_valueModel->m_valueModelRegistry = &valueModelRegistry;
    m_valueModel->m_isReadOnly = isReadOnly;
    m_valueModel->m_isStructureEditable = isStructureEditable;
}
