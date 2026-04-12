/**
 * The factory which creates nodes for values.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/nodeFactory.hpp>

namespace babelwires {
    class TypeSystem;
    struct Context;

    class BABELWIRESQTUI_API ValueNodeFactory : public NodeFactory {
      public:
        ValueNodeFactory(const Context& context);

        QString getCategoryName() const override;

        QList<QString> getFactoryNames() const override;

        void createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF scenePos) override;

      private:
        const TypeSystem& m_typeSystem;
    };

} // namespace babelwires
