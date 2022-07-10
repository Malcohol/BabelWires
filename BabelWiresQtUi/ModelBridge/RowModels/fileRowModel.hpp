/**
 * The row model for a row corresponding to a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp"

namespace babelwires {

    class FileFeature;
    class FileElement;

    /// The file-model is special-cased to query the feature element for the fileName.
    class FileRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual QVariant getTooltip() const override;

        virtual void
        getContextMenuActions(std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const override;

      public:
        const FileFeature& getFileFeature() const;
        const FileElement& getFileElement() const;
    };

} // namespace babelwires
