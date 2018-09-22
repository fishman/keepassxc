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

#include "DatabaseSettingsWidgetEncryption.h"
#include "ui_DatabaseSettingsWidgetEncryption.h"
#include "core/Database.h"
#include "core/Global.h"
#include "core/AsyncTask.h"
#include "gui/MessageBox.h"
#include "crypto/kdf/Argon2Kdf.h"
#include "format/KeePass2.h"

#include <QApplication>
#include <QPushButton>

DatabaseSettingsWidgetEncryption::DatabaseSettingsWidgetEncryption(Database* db, QWidget* parent)
    : QWizardPage(parent)
    , m_ui(new Ui::DatabaseSettingsWidgetEncryption())
    , m_db(db)
{
    m_ui->setupUi(this);

    connect(m_ui->transformBenchmarkButton, SIGNAL(clicked()), SLOT(transformRoundsBenchmark()));
    connect(m_ui->kdfComboBox, SIGNAL(currentIndexChanged(int)), SLOT(kdfChanged(int)));

    connect(m_ui->memorySpinBox, SIGNAL(valueChanged(int)), this, SLOT(memoryChanged(int)));
    connect(m_ui->parallelismSpinBox, SIGNAL(valueChanged(int)), this, SLOT(parallelismChanged(int)));

}

DatabaseSettingsWidgetEncryption::~DatabaseSettingsWidgetEncryption()
{
}

void DatabaseSettingsWidgetEncryption::initializePage()
{
    setupAlgorithmComboBox();
    setupKdfComboBox();
    setupKdfParameterFields();
}

void DatabaseSettingsWidgetEncryption::initializePage(Database* db)
{
    m_db = db;
    initializePage();
}

void DatabaseSettingsWidgetEncryption::setupAlgorithmComboBox()
{
    m_ui->algorithmComboBox->clear();
    for (auto& cipher : asConst(KeePass2::CIPHERS)) {
        m_ui->algorithmComboBox->addItem(QCoreApplication::translate("KeePass2", cipher.second.toUtf8()),
                                         cipher.first.toByteArray());
    }
    int cipherIndex = m_ui->algorithmComboBox->findData(m_db->cipher().toByteArray());
    if (cipherIndex > -1) {
        m_ui->algorithmComboBox->setCurrentIndex(cipherIndex);
    }
}

void DatabaseSettingsWidgetEncryption::setupKdfComboBox()
{
    // Setup kdf combo box
    m_ui->kdfComboBox->blockSignals(true);
    m_ui->kdfComboBox->clear();
    for (auto& kdf : asConst(KeePass2::KDFS)) {
        m_ui->kdfComboBox->addItem(QCoreApplication::translate("KeePass2", kdf.second.toUtf8()),
                                   kdf.first.toByteArray());
    }
    m_ui->kdfComboBox->blockSignals(false);

    int kdfIndex = m_ui->kdfComboBox->findData(m_db->kdf()->uuid().toByteArray());
    if (kdfIndex > -1) {
        m_ui->kdfComboBox->setCurrentIndex(kdfIndex);
        kdfChanged(kdfIndex);
    }
}

void DatabaseSettingsWidgetEncryption::setupKdfParameterFields()
{
    m_ui->memorySpinBox->setValue(64);
    m_ui->parallelismSpinBox->setValue(QThread::idealThreadCount());

    // Setup kdf parameters
    auto kdf = m_db->kdf();
    m_ui->transformRoundsSpinBox->setValue(kdf->rounds());
    if (m_db->kdf()->uuid() == KeePass2::KDF_ARGON2) {
        auto argon2Kdf = kdf.staticCast<Argon2Kdf>();
        m_ui->memorySpinBox->setValue(static_cast<int>(argon2Kdf->memory()) / (1 << 10));
        m_ui->parallelismSpinBox->setValue(argon2Kdf->parallelism());
    }
}

bool DatabaseSettingsWidgetEncryption::validatePage()
{
    // first perform safety check for KDF rounds
    auto kdf = KeePass2::uuidToKdf(Uuid(m_ui->kdfComboBox->currentData().toByteArray()));
    if (kdf->uuid() == KeePass2::KDF_ARGON2 && m_ui->transformRoundsSpinBox->value() > 10000) {
        QMessageBox warning;
        warning.setIcon(QMessageBox::Warning);
        warning.setWindowTitle(tr("Number of rounds too high", "Key transformation rounds"));
        warning.setText(tr("You are using a very high number of key transform rounds with Argon2.\n\n"
                           "If you keep this number, your database may take hours or days (or even longer) to open!"));
        auto ok = warning.addButton(tr("Understood, keep number"), QMessageBox::ButtonRole::AcceptRole);
        auto cancel = warning.addButton(tr("Cancel"), QMessageBox::ButtonRole::RejectRole);
        warning.setDefaultButton(cancel);
        warning.exec();
        if (warning.clickedButton() != ok) {
            return false;
        }
    } else if ((kdf->uuid() == KeePass2::KDF_AES_KDBX3 || kdf->uuid() == KeePass2::KDF_AES_KDBX4)
        && m_ui->transformRoundsSpinBox->value() < 100000) {
        QMessageBox warning;
        warning.setIcon(QMessageBox::Warning);
        warning.setWindowTitle(tr("Number of rounds too low", "Key transformation rounds"));
        warning.setText(tr("You are using a very low number of key transform rounds with AES-KDF.\n\n"
                           "If you keep this number, your database may be too easy to crack!"));
        auto ok = warning.addButton(tr("Understood, keep number"), QMessageBox::ButtonRole::AcceptRole);
        auto cancel = warning.addButton(tr("Cancel"), QMessageBox::ButtonRole::RejectRole);
        warning.setDefaultButton(cancel);
        warning.exec();
        if (warning.clickedButton() != ok) {
            return false;
        }
    }

    m_db->setCipher(Uuid(m_ui->algorithmComboBox->currentData().toByteArray()));

    // Save kdf parameters
    kdf->setRounds(m_ui->transformRoundsSpinBox->value());
    if (kdf->uuid() == KeePass2::KDF_ARGON2) {
        auto argon2Kdf = kdf.staticCast<Argon2Kdf>();
        argon2Kdf->setMemory(static_cast<quint64>(m_ui->memorySpinBox->value()) * (1 << 10));
        argon2Kdf->setParallelism(static_cast<quint32>(m_ui->parallelismSpinBox->value()));
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // TODO: we should probably use AsyncTask::runAndWaitForFuture() here,
    //       but not without making Database thread-safe
    bool ok = m_db->changeKdf(kdf);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        MessageBox::warning(this,
                            tr("KDF unchanged"),
                            tr("Failed to transform key with new KDF parameters; KDF unchanged."),
                            QMessageBox::Ok);
    }
    
    return true;
}

void DatabaseSettingsWidgetEncryption::transformRoundsBenchmark()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_ui->transformBenchmarkButton->setEnabled(false);
    m_ui->transformRoundsSpinBox->setFocus();

    // Create a new kdf with the current parameters
    auto kdf = KeePass2::uuidToKdf(Uuid(m_ui->kdfComboBox->currentData().toByteArray()));
    kdf->setRounds(m_ui->transformRoundsSpinBox->value());
    if (kdf->uuid() == KeePass2::KDF_ARGON2) {
        auto argon2Kdf = kdf.staticCast<Argon2Kdf>();
        if (!argon2Kdf->setMemory(static_cast<quint64>(m_ui->memorySpinBox->value()) * (1 << 10))) {
            m_ui->memorySpinBox->setValue(static_cast<int>(argon2Kdf->memory() / (1 << 10)));
        }
        if (!argon2Kdf->setParallelism(static_cast<quint32>(m_ui->parallelismSpinBox->value()))) {
            m_ui->parallelismSpinBox->setValue(argon2Kdf->parallelism());
        }
    }

    // Determine the number of rounds required to meet 1 second delay
    int rounds = AsyncTask::runAndWaitForFuture([&kdf]() { return kdf->benchmark(1000); });

    m_ui->transformRoundsSpinBox->setValue(rounds);
    m_ui->transformBenchmarkButton->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void DatabaseSettingsWidgetEncryption::kdfChanged(int index)
{
    Uuid id(m_ui->kdfComboBox->itemData(index).toByteArray());

    bool memoryEnabled = id == KeePass2::KDF_ARGON2;
    m_ui->memoryUsageLabel->setEnabled(memoryEnabled);
    m_ui->memorySpinBox->setEnabled(memoryEnabled);

    bool parallelismEnabled = id == KeePass2::KDF_ARGON2;
    m_ui->parallelismLabel->setEnabled(parallelismEnabled);
    m_ui->parallelismSpinBox->setEnabled(parallelismEnabled);

    transformRoundsBenchmark();
}

/**
 * Update memory spin box suffix on value change.
 */
void DatabaseSettingsWidgetEncryption::memoryChanged(int value)
{
    m_ui->memorySpinBox->setSuffix(tr(" MiB", "Abbreviation for Mebibytes (KDF settings)", value));
}

/**
 * Update parallelism spin box suffix on value change.
 */
void DatabaseSettingsWidgetEncryption::parallelismChanged(int value)
{
    m_ui->parallelismSpinBox->setSuffix(tr(" thread(s)", "Threads for parallel execution (KDF settings)", value));
}
