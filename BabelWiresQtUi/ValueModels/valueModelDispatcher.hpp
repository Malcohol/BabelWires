/**
 * Dispatch to the appropriate value model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

namespace babelwires {
    class ValueModelRegistry;

    class ValueModelDispatcher {
      public:
        /// If isStructureEditable is true, then the value is derived from another value.
        /// This is used to prohibit array modification if an array is the target of a connection.
        void init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem, const TypePtr& type, const ValueHolder& value, bool isReadOnly, bool isStructureEditable);

        const ValueModel* operator->() const { return m_valueModel; }

      private:
        ValueModel* m_valueModel;
        ValueModel m_valueModelStorage;
    };
}
