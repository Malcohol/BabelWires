/**
 * QAction which opens an editor for a given value.
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

namespace babelwires {

    struct OpenValueEditorAction : babelwires::FeatureContextMenuAction {
        OpenValueEditorAction(const QString& text, babelwires::FeaturePath pathToArray);

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;

      private:
        babelwires::FeaturePath m_pathToMap;
    };

} // namespace babelwires
