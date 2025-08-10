/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Dialogs/typeInputDialog.hpp>

#include <BabelWiresQtUi/ValueEditors/typeWidget.hpp>

#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>

babelwires::TypeInputDialog::TypeInputDialog(QWidget* parent, const QString& title, const QString& label,
                                             const std::vector<TypeRef>& allowedTypeRefs,
                                             const TypeRef& initialTypeRef, Qt::WindowFlags flags)
    : QDialog(parent) {
    setWindowTitle(title);
    setModal(true);
    setWindowFlags(flags | Qt::WindowType::WindowTitleHint);

    QLabel* const labelWidget = new QLabel(label, this);
    m_typeWidget = new TypeWidget(this, allowedTypeRefs);
    if (initialTypeRef) {
        m_typeWidget->setTypeRef(initialTypeRef);
    }
   
    auto* const buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    QVBoxLayout* const layout = new QVBoxLayout();
    layout->addWidget(labelWidget);
    layout->setAlignment(labelWidget, Qt::AlignLeft);
    layout->addWidget(m_typeWidget);
    layout->setAlignment(m_typeWidget, Qt::AlignLeft);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setLayout(layout);
}

babelwires::TypeRef babelwires::TypeInputDialog::getSelectedType() const {
    return m_typeWidget->getTypeRef();
}

babelwires::TypeRef babelwires::TypeInputDialog::getType(QWidget* parent, const QString& title, const QString& label,
                                                          const std::vector<TypeRef>& allowedTypeRefs,
                                                          const TypeRef& initialTypeRef, bool* ok,
                                                          Qt::WindowFlags flags) {
    TypeInputDialog dialog(parent, title, label, allowedTypeRefs, initialTypeRef, flags);
    if (dialog.exec() == QDialog::Accepted) {
        if (ok) {
            *ok = true;
        }
        return dialog.getSelectedType();
    } else {
        if (ok) {
            *ok = false;
        }
        return TypeRef();
    }
}
