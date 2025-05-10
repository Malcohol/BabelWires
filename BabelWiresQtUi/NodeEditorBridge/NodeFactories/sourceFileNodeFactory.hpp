/**
 * The factory which creates nodes for source files.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/nodeFactory.hpp>

namespace babelwires {
    class SourceFileFormatRegistry;

    class SourceFileNodeFactory : public NodeFactory {
      public:
        SourceFileNodeFactory(const SourceFileFormatRegistry& sourceFileFormatRegistry);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF scenePos) override;

      private:
        const SourceFileFormatRegistry& m_sourceFileFormatRegistry;
    };

} // namespace babelwires
