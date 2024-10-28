/**
 * The RowModelDispatcher provides access to an appropriate RowModel for a row.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/fileRowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/valueRowModel.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

babelwires::RowModelDispatcher::RowModelDispatcher(const ValueModelRegistry& valueModelRegistry,
                                                   const TypeSystem& typeSystem,
                                                   const babelwires::ContentsCacheEntry* entry,
                                                   const babelwires::FeatureElement* element) {
    m_rowModel = &m_rowModelStorage;
    const babelwires::ValueTreeNode* valueTreeNode = &entry->getInputThenOutput()->is<babelwires::ValueTreeNode>();
    if (element->as<FileElement>() && (entry->getDepth() == 0)) {
        assert((valueTreeNode->is<ValueTreeNode>().getType().as<FileType>()) || (valueTreeNode->is<ValueTreeNode>().getType().as<FailureType>()));
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::FileRowModel));
        new (m_rowModel) babelwires::FileRowModel();
    } else {
        static_assert(sizeof(babelwires::RowModel) == sizeof(babelwires::ValueRowModel));
        new (m_rowModel) babelwires::ValueRowModel();
    }
    m_rowModel->m_contentsCacheEntry = entry;
    m_rowModel->m_featureElement = element;
    m_rowModel->init(valueModelRegistry, typeSystem);
}
