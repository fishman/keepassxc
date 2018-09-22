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

#include "gui/DialogyWidget.h"

#include <QScopedPointer>
#include <QPointer>

class Database;
class DatabaseSettingsWidgetGeneral;
class DatabaseSettingsWidgetEncryption;
class DatabaseSettingsWidgetChangeMasterKey;
class QTabWidget;

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
    void showMasterKeySettings();

signals:
    void editFinished(bool accepted);

private slots:
    void save();
    void reject();
    void pageChanged();
    void toggleAdvancedMode(bool advanced);

private:
    const QScopedPointer<Ui::DatabaseSettingsDialog> m_ui;
    QPointer<DatabaseSettingsWidgetGeneral> m_generalWidget;
    QPointer<QTabWidget> m_securityTabWidget;
    QPointer<DatabaseSettingsWidgetChangeMasterKey> m_masterKeyWidget;
    QPointer<DatabaseSettingsWidgetEncryption> m_encryptionWidget;
};

#endif // KEEPASSX_DATABASESETTINGSWIDGET_H
