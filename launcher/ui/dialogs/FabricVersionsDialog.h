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

#include <QDialog>
#include <QSortFilterProxyModel>

#include <BaseVersion.h>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTreeView;

class VersionList;

/**
 * Browses every Fabric loader build, grouped by Minecraft version, with the
 * recommended build per version called out. Read-only: it is meant to answer
 * "what Fabric versions exist for X?" without leaving the launcher.
 */
class FabricVersionsDialog : public QDialog {
    Q_OBJECT

   public:
    explicit FabricVersionsDialog(QWidget* parent = nullptr);
    ~FabricVersionsDialog() override = default;

   protected:
    void closeEvent(QCloseEvent* event) override;

   private:
    void buildUi();
    void refresh();
    void populate();
    void updateCount();

    VersionList* m_list = nullptr;
    QLineEdit* m_search = nullptr;
    QComboBox* m_mcFilter = nullptr;
    QCheckBox* m_releasesOnly = nullptr;
    QTreeView* m_view = nullptr;
    QLabel* m_status = nullptr;
    QLabel* m_count = nullptr;
    QPushButton* m_refreshButton = nullptr;
};

/**
 * Flattens the "Minecraft -> Fabric builds" tree and applies the search box,
 * the Minecraft version selector and the "recommended only" toggle.
 */
class FabricVersionsProxy : public QSortFilterProxyModel {
    Q_OBJECT

   public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    void setSearch(const QString& text);
    void setMinecraftFilter(const QString& mc);
    void setReleasesOnly(bool only);

   protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

   private:
    QString m_search;
    QString m_minecraft;
    bool m_releasesOnly = false;
};
