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

#include "KeyComponentWidget.h"
#include "ui_KeyComponentWidget.h"
#include <QStackedWidget>

KeyComponentWidget::KeyComponentWidget(QWidget* parent)
    : KeyComponentWidget({}, parent)
{
}

KeyComponentWidget::KeyComponentWidget(const QString& name, QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::KeyComponentWidget())
{
    m_ui->setupUi(this);

    connect(m_ui->addButton, SIGNAL(clicked(bool)), SIGNAL(componentAddRequested()));
    connect(m_ui->changeButton, SIGNAL(clicked(bool)), SIGNAL(componentEditRequested()));
    connect(m_ui->removeButton, SIGNAL(clicked(bool)), SIGNAL(componentRemovalRequested()));
    connect(m_ui->cancelButton, SIGNAL(clicked(bool)), SLOT(cancelEdit()));

    connect(m_ui->stackedWidget, SIGNAL(currentChanged(int)), SLOT(reset()));
    connect(m_ui->stackedWidget, SIGNAL(currentChanged(int)), SLOT(updateSize()));

    connect(this, SIGNAL(nameChanged(const QString&)), SLOT(updateComponentName(const QString&)));
    connect(this, SIGNAL(componentAddRequested()), SLOT(doAdd()));
    connect(this, SIGNAL(componentEditRequested()), SLOT(doEdit()));
    connect(this, SIGNAL(componentRemovalRequested()), SLOT(doRemove()));
    connect(this, SIGNAL(componentAddChanged(bool)), SLOT(updateAddStatus(bool)));

    blockSignals(true);
    setComponentName(name);
    m_ui->stackedWidget->setCurrentIndex(Page::AddNew);
    updateSize();
    blockSignals(false);
}

KeyComponentWidget::~KeyComponentWidget()
{
}

/**
 * @param name display name for the key component
 */
void KeyComponentWidget::setComponentName(const QString& name)
{
    if (name == m_componentName) {
        return;
    }

    m_componentName = name;
    emit nameChanged(name);
}

/**
 * @return The key component's display name
 */
QString KeyComponentWidget::componentName() const
{
    return m_componentName;
}

void KeyComponentWidget::setComponentAdded(bool added)
{
    if (m_isComponentAdded == added) {
        return;
    }

    m_isComponentAdded = added;
    emit componentAddChanged(added);
}

bool KeyComponentWidget::componentAdded() const
{
    return m_isComponentAdded;
}

void KeyComponentWidget::changeVisiblePage(KeyComponentWidget::Page page)
{
    m_previousPage = static_cast<Page>(m_ui->stackedWidget->currentIndex());
    m_ui->stackedWidget->setCurrentIndex(page);
}

KeyComponentWidget::Page KeyComponentWidget::visiblePage() const
{
    return static_cast<Page>(m_ui->stackedWidget->currentIndex());
}

void KeyComponentWidget::updateComponentName(const QString& name)
{
    m_ui->addLabel->setText(tr("%1 unset", "Key component unset").arg(name));
    m_ui->addButton->setText(tr("Add %1", "Add a key component").arg(name));
    m_ui->editLabel->setText(tr("Edit %1:", "Edit a key component").arg(name));
    m_ui->componentSetLabel->setText(tr("%1 set", "Key component is set").arg(name));
    m_ui->changeButton->setText(tr("Change %1", "Change a key component").arg(name));
    m_ui->removeButton->setText(tr("Remove %1", "Remove a key component").arg(name));
}

void KeyComponentWidget::updateAddStatus(bool added)
{
    if (added) {
        m_ui->stackedWidget->setCurrentIndex(Page::LeaveOrRemove);
    } else {
        m_ui->stackedWidget->setCurrentIndex(Page::AddNew);
    }
}

void KeyComponentWidget::doAdd()
{
    changeVisiblePage(Page::Edit);
}

void KeyComponentWidget::doEdit()
{
    changeVisiblePage(Page::Edit);
}

void KeyComponentWidget::doRemove()
{
    changeVisiblePage(Page::AddNew);
}

void KeyComponentWidget::cancelEdit()
{
    m_ui->stackedWidget->setCurrentIndex(m_previousPage);
    emit editCanceled();
}

void KeyComponentWidget::reset()
{
    if (static_cast<Page>(m_ui->stackedWidget->currentIndex()) != Page::Edit) {
        return;
    }

    if (!m_ui->componentWidgetLayout->isEmpty()) {
        auto* item = m_ui->componentWidgetLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    m_ui->componentWidgetLayout->addWidget(componentEditWidget());
}

void KeyComponentWidget::updateSize()
{
    for (int i = 0; i < m_ui->stackedWidget->count(); ++i) {
        if (m_ui->stackedWidget->currentIndex() == i) {
            m_ui->stackedWidget->widget(i)->setSizePolicy(
                m_ui->stackedWidget->widget(i)->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
        } else {
            m_ui->stackedWidget->widget(i)->setSizePolicy(
                m_ui->stackedWidget->widget(i)->sizePolicy().horizontalPolicy(), QSizePolicy::Ignored);
        }
    }
    m_ui->stackedWidget->adjustSize();
}
