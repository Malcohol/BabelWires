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
        /// If isSetTypeVariable is true, the action is to set the type variable. If false, it is to unset it.
        SetTypeVariableAction(ProjectDataLocation genericTypeLocation, unsigned int variableIndex, bool isSetTypeVariable);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

        static std::string getActionName(unsigned int variableIndex, bool isSetTypeVariable);

      private:
        ProjectDataLocation m_locationOfGenericType;
        /// Index of the type variable to set.
        unsigned int m_variableIndex;
        /// If true, the action is to set the type variable. If false, it is to unset it.
        bool m_isSetTypeVariable; 
    };

} // namespace babelwires
