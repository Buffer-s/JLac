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
#include "FabricVersionsDialog.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QBrush>
#include <QHash>
#include <QSignalBlocker>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

#include <algorithm>
#include <memory>
#include <ranges>

#include "Application.h"
#include "meta/Index.h"
#include "meta/Version.h"
#include "meta/VersionList.h"
#include "tasks/Task.h"

namespace {
constexpr auto kFabricUid = "net.fabricmc.fabric-loader";

QString describeLoaders(int count)
{
    return count == 1 ? QObject::tr("1 Fabric build") : QObject::tr("%1 Fabric builds").arg(count);
}
}  // namespace

FabricVersionsDialog::FabricVersionsDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Fabric Versions"));
    setWindowIcon(QIcon::fromTheme("fabricmc"));
    resize(760, 560);

    buildUi();
    refresh();
}

void FabricVersionsDialog::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 14);
    layout->setSpacing(10);

    auto* header = new QLabel(this);
    header->setText(QString("<h2 style='margin:0'>%1</h1>").arg(tr("Fabric versions")));
    layout->addWidget(header);

    auto* subtitle = new QLabel(tr("Every Fabric loader build JL knows about, grouped by the Minecraft version it supports."), this);
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    auto* controls = new QHBoxLayout();
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(tr("Search version..."));
    m_search->setClearButtonEnabled(true);
    controls->addWidget(m_search, 1);

    m_mcFilter = new QComboBox(this);
    m_mcFilter->setMinimumWidth(170);
    m_mcFilter->addItem(tr("All Minecraft versions"), QString());
    controls->addWidget(m_mcFilter);

    m_releasesOnly = new QCheckBox(tr("Recommended builds only"), this);
    controls->addWidget(m_releasesOnly);

    m_refreshButton = new QPushButton(tr("Refresh"), this);
    controls->addWidget(m_refreshButton);
    layout->addLayout(controls);

    m_view = new QTreeView(this);
    m_view->setAlternatingRowColors(false);
    m_view->setUniformRowHeights(true);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setAnimated(true);
    m_view->setSortingEnabled(false);
    m_view->header()->setStretchLastSection(true);
    layout->addWidget(m_view, 1);

    auto* footer = new QHBoxLayout();
    m_status = new QLabel(tr("Loading..."), this);
    footer->addWidget(m_status, 1);
    m_count = new QLabel(this);
    footer->addWidget(m_count);
    auto* close = new QPushButton(tr("Close"), this);
    close->setDefault(true);
    footer->addWidget(close);
    layout->addLayout(footer);

    connect(close, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_refreshButton, &QPushButton::clicked, this, &FabricVersionsDialog::refresh);
    connect(m_search, &QLineEdit::textChanged, this, [this](const QString& text) {
        if (auto* proxy = qobject_cast<FabricVersionsProxy*>(m_view->model()); proxy)
            proxy->setSearch(text);
        updateCount();
    });
    connect(m_mcFilter, &QComboBox::currentIndexChanged, this, [this](int) {
        if (auto* proxy = qobject_cast<FabricVersionsProxy*>(m_view->model()); proxy)
            proxy->setMinecraftFilter(m_mcFilter->currentData().toString());
        updateCount();
    });
    connect(m_releasesOnly, &QCheckBox::toggled, this, [this](bool checked) {
        if (auto* proxy = qobject_cast<FabricVersionsProxy*>(m_view->model()); proxy)
            proxy->setReleasesOnly(checked);
        updateCount();
    });
}

void FabricVersionsDialog::refresh()
{
    m_refreshButton->setEnabled(false);
    m_status->setText(tr("Loading..."));
    m_count->clear();

    m_list = APPLICATION->metadataIndex()->get(kFabricUid).get();
    auto task = m_list->getLoadTask(true);
    connect(task.get(), &Task::succeeded, this, &FabricVersionsDialog::populate);
    connect(task.get(), &Task::failed, this, [this](const QString& reason) {
        m_refreshButton->setEnabled(true);
        m_status->setText(tr("Could not load Fabric versions: %1").arg(reason));
    });
    task->start();
}

void FabricVersionsDialog::populate()
{
    m_refreshButton->setEnabled(true);

    auto* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({ tr("Version"), tr("Type"), tr("Released") });

    // Gather builds per Minecraft version first, so we can order the versions
    // numerically (1.21.4 before 1.21.1) instead of alphabetically.
    QHash<QString, std::vector<Meta::Version::Ptr>> byMinecraft;
    QStringList minecraftVersions;
    for (int i = 0; i < m_list->count(); i++) {
        const auto meta = std::dynamic_pointer_cast<Meta::Version>(m_list->at(i));
        if (!meta)
            continue;

        QString mcVersion;
        for (const auto& requirement : meta->requiredSet()) {
            if (requirement.uid == "net.minecraft") {
                mcVersion = requirement.equalsVersion;
                break;
            }
        }
        if (mcVersion.isEmpty())
            mcVersion = tr("Unspecified");

        if (!byMinecraft.contains(mcVersion))
            minecraftVersions.append(mcVersion);
        byMinecraft[mcVersion].push_back(meta);
    }

    auto descending = [](const QString& a, const QString& b) { return Version(a) > Version(b); };
    std::sort(minecraftVersions.begin(), minecraftVersions.end(), descending);

    auto* root = model->invisibleRootItem();
    const QString previousFilter = m_mcFilter->currentData().toString();

    for (const auto& mcVersion : minecraftVersions) {
        auto builds = byMinecraft.value(mcVersion);
        std::sort(builds.begin(), builds.end(), [](const Meta::Version::Ptr& a, const Meta::Version::Ptr& b) {
            return a->version() > b->version();
        });

        auto* parent = new QStandardItem(mcVersion);
        parent->setEditable(false);
        const bool hasRecommended = std::ranges::any_of(builds, [](const auto& v) { return v->isRecommended(); });
        parent->setToolTip(tr("Fabric builds for Minecraft %1").arg(mcVersion));

        for (const auto& meta : builds) {
            auto* name = new QStandardItem(meta->version());
            name->setEditable(false);
            name->setData(meta->isRecommended(), BaseVersionList::RecommendedRole);

            const QString label = meta->isRecommended() ? tr("Recommended") : tr("Build");
            auto* type = new QStandardItem(label);
            type->setEditable(false);

            auto* date = new QStandardItem(meta->time().toString(Qt::ISODate));
            date->setEditable(false);

            parent->appendRow({ name, type, date });
        }

        if (hasRecommended)
            parent->setForeground(QBrush(QColor("#F2F2F3")));
        root->appendRow(parent);
    }

    auto* proxy = new FabricVersionsProxy(this);
    proxy->setSourceModel(model);
    m_view->setModel(proxy);
    m_view->expandAll();
    m_view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    // Keep the Minecraft selector in sync without losing the user's choice.
    {
        QSignalBlocker blocker(m_mcFilter);
        m_mcFilter->clear();
        m_mcFilter->addItem(tr("All Minecraft versions"), QString());
        for (const auto& mcVersion : minecraftVersions)
            m_mcFilter->addItem(tr("Minecraft %1").arg(mcVersion), mcVersion);
        const int index = m_mcFilter->findData(previousFilter);
        m_mcFilter->setCurrentIndex(index >= 0 ? index : 0);
    }

    m_status->setText(tr("Updated just now"));
    updateCount();
}

void FabricVersionsDialog::updateCount()
{
    QAbstractItemModel* model = m_view->model();
    if (!model)
        return;

    int builds = 0;
    for (int i = 0; i < model->rowCount(); i++) {
        if (!model->hasChildren(model->index(i, 0)))
            continue;
        builds += model->rowCount(model->index(i, 0));
    }
    m_count->setText(describeLoaders(builds));
}

void FabricVersionsDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);
}

// ---------------------------------------------------------------------------
// Proxy
// ---------------------------------------------------------------------------

void FabricVersionsProxy::setSearch(const QString& text)
{
    m_search = text.trimmed();
    invalidateFilter();
}

void FabricVersionsProxy::setMinecraftFilter(const QString& mc)
{
    m_minecraft = mc;
    invalidateFilter();
}

void FabricVersionsProxy::setReleasesOnly(bool only)
{
    m_releasesOnly = only;
    invalidateFilter();
}

bool FabricVersionsProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    // Top-level rows are Minecraft versions.
    if (!sourceParent.isValid()) {
        if (!m_minecraft.isEmpty() && index.data(Qt::DisplayRole).toString() != m_minecraft)
            return false;

        // Keep a Minecraft version visible if any of its builds pass the filters,
        // or if it matches the search text itself.
        if (!m_search.isEmpty() && index.data(Qt::DisplayRole).toString().contains(m_search, Qt::CaseInsensitive))
            return true;

        const int children = sourceModel()->rowCount(index);
        for (int i = 0; i < children; i++) {
            if (filterAcceptsRow(i, index))
                return true;
        }
        return m_search.isEmpty() && !m_releasesOnly;
    }

    const QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    if (m_releasesOnly && !nameIndex.data(BaseVersionList::RecommendedRole).toBool())
        return false;
    if (!m_search.isEmpty() && !nameIndex.data(Qt::DisplayRole).toString().contains(m_search, Qt::CaseInsensitive))
        return false;
    return true;
}
