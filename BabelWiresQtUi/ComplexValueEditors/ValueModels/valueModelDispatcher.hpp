/**
 * Dispatch to the appropriate value model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModel.hpp>

namespace babelwires {
    class ValueModelDispatcher {
      public:
        void init(/*const ValueModelRegistry& valueModelRegistry, */ const Type& type, const Value& value);

        const ValueModel* operator->() const { return m_valueModel; }

      private:
        ValueModel* m_valueModel;
        ValueModel m_valueModelStorage;
    };
}
