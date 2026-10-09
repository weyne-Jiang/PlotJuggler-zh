/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

#include "language_manager.h"
#include "toolbox_display_name.h"
#include "transform_display_name.h"

class LanguageManagerTest : public QObject
{
  Q_OBJECT
private slots:
  void transformLabelsKeepIdentifiersStable()
  {
    const QStringList names = { "Absolute",
                                "Binary Filter",
                                "Derivative",
                                "Integral",
                                "Moving Average",
                                "Moving Root Mean Squared",
                                "Moving Variance / Stdev",
                                "Outlier Removal",
                                "Samples Counter",
                                "Scale/Offset" };
    PJ::LanguageManager language;
    QVERIFY(language.install("zh_CN"));
    for (const auto& name : names)
    {
      QVERIFY(PJ::transformDisplayName(name) != name);
    }
    QCOMPARE(PJ::transformDisplayName("Custom transform"), QString("Custom transform"));
    QVERIFY(language.install("en"));
    for (const auto& name : names)
    {
      QCOMPARE(PJ::transformDisplayName(name), name);
    }
  }

  void toolboxLabelsKeepPluginIdentifiersStable()
  {
    const QStringList names = { "CSV/Parquet Exporter", "Fast Fourier Transform",
                                "Quaternion to RPY", "Reactive Script Editor" };
    const QStringList labels = { QStringLiteral("CSV/Parquet 导出器"),
                                 QStringLiteral("快速傅里叶变换"),
                                 QStringLiteral("四元数转滚转/俯仰/偏航角"),
                                 QStringLiteral("响应式脚本编辑器") };
    PJ::LanguageManager language;
    QVERIFY(language.install("zh_CN"));
    for (int i = 0; i < names.size(); ++i)
    {
      QCOMPARE(PJ::toolboxDisplayName(names[i]), labels[i]);
    }
    QCOMPARE(PJ::toolboxDisplayName("Third-party toolbox"), QString("Third-party toolbox"));
    QVERIFY(language.install("en"));
    for (const auto& name : names)
    {
      QCOMPARE(PJ::toolboxDisplayName(name), name);
    }
  }

  void resolvesChoicesAndSystemScripts_data()
  {
    QTest::addColumn<QString>("choice");
    QTest::addColumn<QString>("locale");
    QTest::addColumn<QString>("expected");
    QTest::newRow("explicit-English") << "en"
                                      << "zh_CN"
                                      << "en";
    QTest::newRow("explicit-Chinese") << "zh_CN"
                                      << "en_US"
                                      << "zh_CN";
    QTest::newRow("system-CN") << "system"
                               << "zh_CN"
                               << "zh_CN";
    QTest::newRow("system-SG") << "system"
                               << "zh_SG"
                               << "zh_CN";
    QTest::newRow("system-TW") << "system"
                               << "zh_TW"
                               << "en";
    QTest::newRow("system-US") << "system"
                               << "en_US"
                               << "en";
    QTest::newRow("invalid-CN") << "unknown"
                                << "zh_CN"
                                << "zh_CN";
    QTest::newRow("invalid-US") << "zh_TW"
                                << "en_US"
                                << "en";
  }

  void resolvesChoicesAndSystemScripts()
  {
    QFETCH(QString, choice);
    QFETCH(QString, locale);
    QFETCH(QString, expected);
    QCOMPARE(PJ::LanguageManager::resolveLanguage(choice, QLocale(locale)), expected);
  }

  void settingsDefaultInvalidAndReopen()
  {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString file = directory.filePath("preferences.ini");
    {
      QSettings settings(file, QSettings::IniFormat);
      QCOMPARE(PJ::LanguageManager::savedPreference(settings), QString("system"));
      settings.setValue(PJ::LanguageManager::kSettingsKey, "invalid");
      QCOMPARE(PJ::LanguageManager::savedPreference(settings), QString("system"));
      settings.setValue(PJ::LanguageManager::kSettingsKey, "zh_CN");
      settings.sync();
    }
    QSettings reopened(file, QSettings::IniFormat);
    QCOMPARE(PJ::LanguageManager::savedPreference(reopened), QString("zh_CN"));
  }

  void installsEmbeddedCataloguesAndReleasesThem()
  {
    {
      PJ::LanguageManager language;
      QVERIFY(language.install("zh_CN"));
      const QString label = QCoreApplication::translate("PreferencesDialog", "Language:");
      QVERIFY2(label.contains(QStringLiteral("语言")), qPrintable(label));
      QCOMPARE(QCoreApplication::translate("QDialogButtonBox", "Cancel"), QStringLiteral("取消"));
      QVERIFY(language.install("en", QLocale("zh_CN")));
      QCOMPARE(QCoreApplication::translate("PreferencesDialog", "Language:"), QString("Language:"));
      QVERIFY(language.install("zh_CN"));
    }
    QCOMPARE(QCoreApplication::translate("PreferencesDialog", "Language:"), QString("Language:"));
  }

  void missingResourceFallsBackToEnglish()
  {
    PJ::LanguageManager language(":/missing-i18n");
    QTest::ignoreMessage(QtWarningMsg,
                         "Chinese translation resources could not be loaded; using English");
    QVERIFY(!language.install("zh_CN"));
    QCOMPARE(QCoreApplication::translate("PreferencesDialog", "Language:"), QString("Language:"));
  }
};

QTEST_GUILESS_MAIN(LanguageManagerTest)
#include "language_manager_test.moc"
