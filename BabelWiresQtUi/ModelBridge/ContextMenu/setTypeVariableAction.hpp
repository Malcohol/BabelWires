/**
 * QAction which opens a TypeInputDialog to set a type variable.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>

namespace babelwires {
 
    struct SetTypeVariableAction : babelwires::NodeContentsContextMenuActionBase {
        SetTypeVariableAction(ProjectDataLocation genericTypeLocation, unsigned int variableIndex);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

        static std::string getActionName(unsigned int variableIndex);

      private:
        ProjectDataLocation m_locationOfGenericType;
        unsigned int m_variableIndex; // Index of the type variable to set.
    };

} // namespace babelwires
