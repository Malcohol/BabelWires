/**
 * QAction for activating or deactivating optional fields.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWires/Features/Path/featurePath.hpp"
#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"

namespace babelwires {

    /// QAction for activating or deactivating an optional.
    class OptionalActivationAction : public babelwires::FeatureContextMenuAction {
      public:
        OptionalActivationAction(babelwires::FeaturePath pathToRecord, FieldIdentifier optional, bool isActivated);

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;

      private:
        babelwires::FeaturePath m_pathToRecord;
        FieldIdentifier m_optional;
        bool m_isActivated;
    };
}
