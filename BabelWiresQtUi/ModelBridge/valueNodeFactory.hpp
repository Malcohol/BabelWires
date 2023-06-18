/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    class ProjectBridge;
    
    class ValueNodeFactory : public NodeFactoryBase {
      public:
        ValueNodeFactory(ProjectBridge* projectBridge, TypeRef typeOfValue);

        QString name() const override;
        std::unique_ptr<QtNodes::NodeDataModel> createNode() const;

      private:
        TypeRef m_typeOfValue;
    };

} // namespace babelwires
