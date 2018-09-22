/*
 *  Copyright (C) 2012 Felix Geyer <debfx@fobos.de>
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
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

#ifndef KEEPASSX_CHANGEMASTERKEYWIDGET_H
#define KEEPASSX_CHANGEMASTERKEYWIDGET_H

#include <QScopedPointer>

#include "gui/DialogyWidget.h"
#include "keys/CompositeKey.h"

class QLabel;
namespace Ui
{
    class ChangeMasterKeyWidget;
}

class ChangeMasterKeyWidget : public DialogyWidget
{
    Q_OBJECT

public:
    explicit ChangeMasterKeyWidget(QWidget* parent = nullptr);
    ~ChangeMasterKeyWidget();
    void clearForms();
    QSharedPointer<CompositeKey> newMasterKey();
    QLabel* headlineLabel();

public slots:
    void setCancelEnabled(bool enabled);

signals:
    void editFinished(bool accepted);

private slots:
    void generateKey();
    void reject();

private:
    const QScopedPointer<Ui::ChangeMasterKeyWidget> m_ui;
    CompositeKey m_key;

    Q_DISABLE_COPY(ChangeMasterKeyWidget)
};

#endif // KEEPASSX_CHANGEMASTERKEYWIDGET_H
