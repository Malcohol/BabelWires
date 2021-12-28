/**
 * QAction which opens the map editor for a given map.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/Path/featurePath.hpp"
#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"

namespace babelwires {

    struct EditMapAction : babelwires::FeatureContextMenuAction {
        EditMapAction(babelwires::FeaturePath pathToArray);

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;

      private:
        babelwires::FeaturePath m_pathToMap;
    };

} // namespace babelwires
