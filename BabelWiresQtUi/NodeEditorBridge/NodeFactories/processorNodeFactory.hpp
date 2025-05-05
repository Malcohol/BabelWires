/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/Factories/nodeFactory.hpp>

namespace babelwires {
    class ProcessorFactoryRegistry;

    class ProcessorNodeFactory : public NodeFactory {
      public:
        ProcessorNodeFactory(const ProcessorFactoryRegistry& processorFactoryRegistry);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF const scenePos, QWidget* parentForDialogs) override;

      private:
        const ProcessorFactoryRegistry& m_processorFactoryRegistry;
    };

} // namespace babelwires
