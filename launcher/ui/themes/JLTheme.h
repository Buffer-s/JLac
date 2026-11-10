// SPDX-License-Identifier: GPL-3.0-only
/*
 *  JL - Minecraft Launcher
 *  Copyright (C) 2026 Buffer
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "ITheme.h"

/**
 * The default JL look: near-black surfaces, a single white accent and flat,
 * borderless controls. Kept deliberately small so it feels like an application
 * theme rather than a skin.
 */
class JLTheme : public ITheme {
   public:
    ~JLTheme() override = default;

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QString qtTheme() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
