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
    class ProjectBridge;
    class TargetFileFormat;

    class TargetFileNodeFactory {
      public:
        TargetFileNodeFactory(ProjectBridge* projectBridge, const TargetFileFormat* targetFileFormat);

        QString name() const;
        std::unique_ptr<QtNodes::NodeDataModel> operator()() const;

      private:
        ProjectBridge* m_projectBridge;
        const TargetFileFormat* m_targetFileFormat;
        mutable bool m_queryHack = false;
    };

} // namespace babelwires
