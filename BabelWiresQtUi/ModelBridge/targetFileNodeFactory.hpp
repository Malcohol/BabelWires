/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

namespace babelwires {
    class ProjectGraphModel;
    class TargetFileFormat;

    class TargetFileNodeFactory : public NodeFactoryBase {
      public:
        TargetFileNodeFactory(ProjectGraphModel* projectGraphModel, const TargetFileFormat* targetFileFormat);

        QString name() const override;
        std::unique_ptr<QtNodes::NodeDataModel> createNode() const;

      private:
        const TargetFileFormat* m_targetFileFormat;
    };

} // namespace babelwires
