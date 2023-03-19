/**
 * The RowModelDispatcher provides access to an appropriate RowModel for a row.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/arrayRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/fileRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/intRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/rationalRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/recordWithOptionalsRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelRegistry.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/stringRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/enumRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/mapRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/unionRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/valueRowModel.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/stringFeature.hpp>
#include <BabelWiresLib/Features/enumFeature.hpp>
#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/FileFormat/fileFeature.hpp>

babelwires::RowModelDispatcher::RowModelDispatcher(const RowModelRegistry& rowModelRegistry,
                                                   const babelwires::ContentsCacheEntry* entry,
                                                   const babelwires::FeatureElement* element) {
    m_rowModel = &m_rowModelStorage;
    const babelwires::Feature* feature = entry->getInputThenOutputFeature();
    if (rowModelRegistry.handleFeature(feature, m_rowModel)) {
        // Handled by a registered handler.
    } else if (feature->as<const babelwires::SimpleValueFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::ValueRowModel));
        new (m_rowModel) babelwires::ValueRowModel();
    } else if (feature->as<const babelwires::StringFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::StringRowModel));
        new (m_rowModel) babelwires::StringRowModel();
    } else if (feature->as<const babelwires::IntFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::IntRowModel));
        new (m_rowModel) babelwires::IntRowModel();
    } else if (feature->as<const babelwires::RationalFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::RationalRowModel));
        new (m_rowModel) babelwires::RationalRowModel();
    } else if (feature->as<const babelwires::EnumFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::EnumRowModel));
        new (m_rowModel) babelwires::EnumRowModel();
    } else if (feature->as<const babelwires::ArrayFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::ArrayRowModel));
        new (m_rowModel) babelwires::ArrayRowModel();
    } else if (feature->as<const babelwires::FileFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::FileRowModel));
        new (m_rowModel) babelwires::FileRowModel();
    } else if (feature->as<const babelwires::RecordWithOptionalsFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::RecordWithOptionalsRowModel));
        new (m_rowModel) babelwires::RecordWithOptionalsRowModel();
    } else if (feature->as<const babelwires::MapFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::MapRowModel));
        new (m_rowModel) babelwires::MapRowModel();
    } else if (feature->as<const babelwires::UnionFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::UnionRowModel));
        new (m_rowModel) babelwires::UnionRowModel();
    } else {
        // The base row model is used.
    }
    m_rowModel->m_contentsCacheEntry = entry;
    m_rowModel->m_featureElement = element;
    m_rowModel->init();
}
