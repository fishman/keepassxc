/*
 *  Copyright (C) 2012 Felix Geyer <debfx@fobos.de>
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

#ifndef KEEPASSX_DATABASESETTINGSWIDGET_H
#define KEEPASSX_DATABASESETTINGSWIDGET_H

#include <QScopedPointer>
#include "gui/DialogyWidget.h"

class Database;
class DatabaseSettingsWidgetGeneral;
class DatabaseSettingsWidgetEncryption;

namespace Ui
{
    class DatabaseSettingsDialog;
}

class DatabaseSettingsDialog : public DialogyWidget
{
    Q_OBJECT

public:
    explicit DatabaseSettingsDialog(QWidget* parent = nullptr);
    ~DatabaseSettingsDialog() override;
    Q_DISABLE_COPY(DatabaseSettingsDialog);

    void load(Database* db);

signals:
    void editFinished(bool accepted);

private slots:
    void save();
    void reject();

private:
    const QScopedPointer<Ui::DatabaseSettingsDialog> m_ui;
    DatabaseSettingsWidgetGeneral* m_generalPage;
    DatabaseSettingsWidgetEncryption* m_encryptionPage;
};

#endif // KEEPASSX_DATABASESETTINGSWIDGET_H
