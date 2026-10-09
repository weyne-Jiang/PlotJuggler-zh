/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

#include "preferences_dialog.h"

class PreferencesLanguageTest : public QObject
{
  Q_OBJECT
private slots:
  void init()
  {
    QVERIFY(directory_.isValid());
    QCoreApplication::setOrganizationName("PlotJugglerLanguageTests");
    QCoreApplication::setApplicationName("Preferences");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory_.path());
    QSettings settings;
    settings.clear();
    settings.setValue("Preferences::language", "en");
    settings.sync();
  }

  void cancelLeavesSavedLanguageUnchanged()
  {
    PreferencesDialog dialog;
    auto* combo = dialog.findChild<QComboBox*>("comboBoxLanguage");
    QVERIFY2(combo, "Appearance must provide the language selector");
    QCOMPARE(combo->currentData().toString(), QString("en"));
    QCOMPARE(combo->count(), 3);
    auto* hint = dialog.findChild<QLabel*>("labelLanguageRestart");
    QVERIFY(hint);
    QVERIFY(hint->isHidden());
    combo->setCurrentIndex(combo->findData("zh_CN"));
    QVERIFY(!hint->isHidden());
    combo->setCurrentIndex(combo->findData("en"));
    QVERIFY(hint->isHidden());
    combo->setCurrentIndex(combo->findData("zh_CN"));
    auto* buttons = dialog.findChild<QDialogButtonBox*>("buttonBox");
    QVERIFY(buttons);
    QTest::mouseClick(buttons->button(QDialogButtonBox::Cancel), Qt::LeftButton);
    QCOMPARE(QSettings().value("Preferences::language").toString(), QString("en"));
  }

  void okPersistsLanguageForNextDialog()
  {
    PreferencesDialog dialog;
    auto* combo = dialog.findChild<QComboBox*>("comboBoxLanguage");
    QVERIFY2(combo, "Appearance must provide the language selector");
    combo->setCurrentIndex(combo->findData("zh_CN"));
    auto* buttons = dialog.findChild<QDialogButtonBox*>("buttonBox");
    QVERIFY(buttons);
    QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
    QSettings().sync();
    QCOMPARE(QSettings().value("Preferences::language").toString(), QString("zh_CN"));
    PreferencesDialog reopened;
    auto* restored = reopened.findChild<QComboBox*>("comboBoxLanguage");
    QVERIFY(restored);
    QCOMPARE(restored->currentData().toString(), QString("zh_CN"));
  }

private:
  QTemporaryDir directory_;
};

QTEST_MAIN(PreferencesLanguageTest)
#include "preferences_language_test.moc"
