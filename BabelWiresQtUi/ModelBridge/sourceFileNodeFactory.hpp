/**
 * The factory which creates nodes for source files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ModelBridge/baseNodeModel.hpp"

namespace babelwires {
    class ProjectBridge;
    class SourceFileFormat;

    /// The factory which creates nodes for source files.
    class SourceFileNodeFactory {
      public:
        SourceFileNodeFactory(ProjectBridge* projectBridge, const SourceFileFormat* sourceFileFormat);

        QString name() const;
        std::unique_ptr<QtNodes::NodeDataModel> operator()() const;

      private:
        ProjectBridge* m_projectBridge;
        const SourceFileFormat* m_sourceFileFormat;
        mutable bool m_queryHack = false;
    };

} // namespace babelwires
