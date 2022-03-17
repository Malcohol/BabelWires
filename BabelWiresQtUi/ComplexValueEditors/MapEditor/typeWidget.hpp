/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Features/mapFeature.hpp>

#include <QComboBox>

#include <unordered_set>

namespace babelwires {
    class ProjectBridge;
    class MapFeature;

    class TypeWidget : public QComboBox {
        Q_OBJECT
      public:
        TypeWidget(QWidget* parent, ProjectBridge& projectBridge, const MapFeature::TypeSet& typeIds);
    };
}