/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/Factories/nodeFactory.hpp>

namespace babelwires {
    class TargetFileFormatRegistry;

    class TargetFileNodeFactory : public NodeFactory {
      public:
        TargetFileNodeFactory(const TargetFileFormatRegistry& targetFileFormatRegistry);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF const scenePos,
                        QWidget* parentForDialogs) override;

      private:
        const TargetFileFormatRegistry& m_targetFileFormatRegistry;
    };

} // namespace babelwires
