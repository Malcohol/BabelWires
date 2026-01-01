/**
 * A dialog for selecting a type from the TypeSystem.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeExp.hpp>

#include <QDialog>

namespace babelwires {
    class TypeWidget;

    /// A dialog for selecting a type from the TypeSystem.
    class TypeInputDialog : public QDialog {
        Q_OBJECT
      public:
        TypeInputDialog(QWidget* parent, const QString& title, const QString& label, const std::vector<TypeExp>& allowedTypeExps, const TypeExp& initialTypeExp, Qt::WindowFlags flags);

        /// Get the selected type.
        TypeExp getSelectedType() const;

        static TypeExp getType(QWidget* parent, const QString& title, const QString& label,
                                 const std::vector<TypeExp>& allowedTypeExps, const TypeExp& initialTypeExp, bool* ok = nullptr,
                                 Qt::WindowFlags flags = Qt::WindowFlags());

      private:
        TypeWidget* m_typeWidget;
    };

} // namespace babelwires
