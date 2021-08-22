/**
 * The RowModelDispatcher provides access to an appropriate RowModel for a row.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp"

#include "BabelWiresQtUi/ModelBridge/RowModels/arrayRowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/fileRowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/intRowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/rationalRowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/recordWithOptionalsRowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/rowModelRegistry.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/stringRowModel.hpp"

#include "BabelWires/Project/FeatureElements/featureElement.hpp"

#include "BabelWires/Features/arrayFeature.hpp"
#include "BabelWires/Features/numericFeature.hpp"
#include "BabelWires/Features/recordWithOptionalsFeature.hpp"
#include "BabelWires/Features/stringFeature.hpp"
#include "BabelWires/FileFormat/fileFeature.hpp"

babelwires::RowModelDispatcher::RowModelDispatcher(const RowModelRegistry& rowModelRegistry,
                                                   const babelwires::ContentsCacheEntry* entry,
                                                   const babelwires::FeatureElement* element) {
    m_rowModel = &m_rowModelStorage;
    const babelwires::Feature* feature = entry->getInputThenOutputFeature();
    if (rowModelRegistry.handleFeature(feature, m_rowModel)) {
        // Handled by a registered handler.
    } else if (feature->as<const babelwires::StringFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::StringRowModel));
        new (m_rowModel) babelwires::StringRowModel();
    } else if (feature->as<const babelwires::IntFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::IntRowModel));
        new (m_rowModel) babelwires::IntRowModel();
    } else if (feature->as<const babelwires::RationalFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::RationalRowModel));
        new (m_rowModel) babelwires::RationalRowModel();
    } else if (feature->as<const babelwires::ArrayFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::ArrayRowModel));
        new (m_rowModel) babelwires::ArrayRowModel();
    } else if (feature->as<const babelwires::FileFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::FileRowModel));
        new (m_rowModel) babelwires::FileRowModel();
    } else if (feature->as<const babelwires::RecordWithOptionalsFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::RecordWithOptionalsRowModel));
        new (m_rowModel) babelwires::RecordWithOptionalsRowModel();
    } else {
        // The base row model is used.
    }
    m_rowModel->m_contentsCacheEntry = entry;
    m_rowModel->m_featureElement = element;
}
