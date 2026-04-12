/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/nodeFactory.hpp>

namespace babelwires {
    class ProcessorFactoryRegistry;
    struct Context;

    class BABELWIRESQTUI_API ProcessorNodeFactory : public NodeFactory {
      public:
        ProcessorNodeFactory(const Context& context);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF scenePos) override;

      private:
        const ProcessorFactoryRegistry& m_processorFactoryRegistry;
    };

} // namespace babelwires
