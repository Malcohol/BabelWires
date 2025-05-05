/** 
 * 
 * Copyright (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#pragma once

#include <QtNodes/BasicGraphicsScene>

#include <QMenu>

namespace babelwires {

    class ProjectGraphModel;
    struct ProjectContext;

    class ProjectGraphicsScene : public QtNodes::BasicGraphicsScene {
      public:
        ProjectGraphicsScene(ProjectGraphModel& graphModel, const ProjectContext& projectContext);

        QMenu* createSceneMenu(QPointF const scenePos) override;
    };

} // namespace babelwires