/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef PJ_LANGUAGE_MANAGER_H
#define PJ_LANGUAGE_MANAGER_H

#include <QLocale>
#include <QTranslator>

class QSettings;

namespace PJ
{
/// Owns translators installed before any application window is constructed.
/// Saved preference changes take effect on the next launch.
class LanguageManager final
{
public:
  static constexpr auto kSettingsKey = "Preferences::language";

  explicit LanguageManager(QString resource_root = QStringLiteral(":/i18n"));
  ~LanguageManager();
  LanguageManager(const LanguageManager&) = delete;
  LanguageManager& operator=(const LanguageManager&) = delete;

  static QString normalizePreference(const QString& preference);
  static QString savedPreference(const QSettings& settings);
  static QString resolveLanguage(const QString& preference, const QLocale& system_locale);

  /// Returns false on resource failure, leaving the English source UI usable.
  bool install(const QString& preference, const QLocale& system_locale = QLocale::system());

private:
  QString resource_root_;
  QTranslator qt_translator_;
  QTranslator app_translator_;
};
}  // namespace PJ

#endif
