/**
 * The factory which creates nodes for values.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/Factories/nodeFactory.hpp>

namespace babelwires {
    class TypeSystem;

    class ValueNodeFactory : public NodeFactory {
      public:
        ValueNodeFactory(const TypeSystem& typeSystem);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF const scenePos, QWidget* parentForDialogs) override;

      private:
        const TypeSystem& m_typeSystem;
    };

} // namespace babelwires
