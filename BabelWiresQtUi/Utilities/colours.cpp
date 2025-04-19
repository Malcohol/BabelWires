/**
 * Some common colour handling routines.
 *
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Utilities/colours.hpp>

#include <QApplication>
#include <QPalette>

QColor babelwires::getBackgroundColour(BackgroundType type) {
    const QColor baseColor = QApplication::palette().color(QPalette::Button);
    if (type == BackgroundType::normal) {
        return baseColor;
    }
    int r, g, b;
    baseColor.getRgb(&r, &g, &b);
    const int gSoft = std::min(g, 255-g);
    const int bSoft = std::min(b, 255-b);
    const int rSoft = std::min(gSoft, bSoft);
    r = 255 - rSoft;
    g = (type == BackgroundType::failure) ? gSoft : 127;
    b = (type == BackgroundType::failure) ? bSoft : 127; 
    return QColor(r, g, b);
}