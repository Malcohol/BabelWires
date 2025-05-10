/**
 *
 */
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <QList>
#include <QString>
#include <QPointF>

namespace babelwires {
    class ProjectGraphModel;

    class NodeFactory {
      public:
        virtual ~NodeFactory();

        virtual QString getCategoryName() const = 0;

        virtual QList<QString> getFactoryNames() const = 0;

        virtual void createNode(ProjectGraphModel& projectGraphicsScene, QString factoryName, QPointF const scenePos) = 0;
    };
} // namespace babelwires
