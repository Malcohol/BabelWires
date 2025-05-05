/**
 * 
 */

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectGraphModel;

    class NodeFactory {
        virtual ~NodeFactory();

        virtual QString getCategoryName() const = 0;

        virtual QList<QString> getFactoryNames() const = 0;

        virtual void createNode(ProjectGraphModel& projectGraphicsScene, QString factoryName, QPointF const scenePos, QWidget* parentForDialogs) = 0;
    };
}
