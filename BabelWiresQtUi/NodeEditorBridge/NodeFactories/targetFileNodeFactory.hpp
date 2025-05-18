/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/nodeFactory.hpp>

namespace babelwires {
    class TargetFileFormatRegistry;
    struct UiProjectContext;

    class TargetFileNodeFactory : public NodeFactory {
      public:
        TargetFileNodeFactory(const UiProjectContext& uiProjectContext);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF scenePos) override;

      private:
        const TargetFileFormatRegistry& m_targetFileFormatRegistry;
    };

} // namespace babelwires
