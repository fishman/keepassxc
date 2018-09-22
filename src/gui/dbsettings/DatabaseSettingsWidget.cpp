/*
 *  Copyright (C) 2018 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DatabaseSettingsWidget.h"
#include "core/Database.h"

#include <QWidget>
#include <QTimer>

DatabaseSettingsWidget::DatabaseSettingsWidget(QWidget* parent)
    : SettingsWidget(parent)
{
}

DatabaseSettingsWidget::~DatabaseSettingsWidget()
{
}

/**
 * Load the database to be configured by this page and initialize the page.
 * The page will NOT take ownership of the database.
 *
 * @param db database object to be configured
 */
void DatabaseSettingsWidget::load(Database* db)
{
    m_db = db;
    initialize();
}

/**
 * Queue a focus change to the specified element to be executed
 * in the next event loop iteration.
 *
 * @param widget widget to be focused
 */
void DatabaseSettingsWidget::queueFocus(QWidget* widget)
{
    // ugly verbose code needed for compatibility with Qt < 5.4,
    // see https://bugreports.qt.io/browse/QTBUG-26406
    auto* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [=]() {
        if (widget) {
            widget->setFocus();
        }
        timer->deleteLater();
    });
    timer->start(0);
}
