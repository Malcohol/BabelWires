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
#include <BabelWiresQtUi/ModelBridge/RowModels/recordWithOptionalsRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelRegistry.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/valueRowModel.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Types/Enum/enumFeature.hpp>

babelwires::RowModelDispatcher::RowModelDispatcher(const RowModelRegistry& rowModelRegistry,
                                                   const ValueModelRegistry& valueModelRegistry,
                                                   const TypeSystem& typeSystem,
                                                   const babelwires::ContentsCacheEntry* entry,
                                                   const babelwires::FeatureElement* element) {
    m_rowModel = &m_rowModelStorage;
    const babelwires::Feature* feature = entry->getInputThenOutputFeature();
    if (rowModelRegistry.handleFeature(feature, m_rowModel)) {
        // Handled by a registered handler.
    } else if (feature->as<const babelwires::ValueFeature>()) {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::ValueRowModel));
        new (m_rowModel) babelwires::ValueRowModel();
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
    m_rowModel->init(valueModelRegistry, typeSystem);
}
