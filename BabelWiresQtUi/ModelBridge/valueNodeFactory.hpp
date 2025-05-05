/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    class ProjectGraphModel;
    
    class ValueNodeFactory : public NodeFactoryBase {
      public:
        ValueNodeFactory(ProjectGraphModel* projectGraphModel, TypeRef typeOfValue);

        QString name() const override;
        std::unique_ptr<QtNodes::NodeDataModel> createNode() const;

      private:
        TypeRef m_typeOfValue;
    };

} // namespace babelwires
