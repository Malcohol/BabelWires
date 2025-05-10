/** 
 * 
 * Copyright (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/nodeFactory.hpp>

#include <QtNodes/BasicGraphicsScene>

#include <QMenu>

namespace babelwires {

    class ProjectGraphModel;

    class ProjectGraphicsScene : public QtNodes::BasicGraphicsScene {
      public:
        ProjectGraphicsScene(ProjectGraphModel& graphModel);

        QMenu* createSceneMenu(QPointF const scenePos) override;

        void setWidgetForDialogs(QWidget* widgetForDialogs);

      private:
        void addNodeFactory(std::unique_ptr<NodeFactory> nodeFactory);
        
      private:
        std::vector<std::unique_ptr<NodeFactory>> m_nodeFactories;
        QWidget* m_widgetForDialogs = nullptr;
    };

} // namespace babelwires
