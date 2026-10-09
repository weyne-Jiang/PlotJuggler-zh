/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "language_manager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <utility>

// Generated resource symbols must resolve in the global namespace.
static void initializeLanguageResources()
{
  Q_INIT_RESOURCE(plotjuggler_i18n);
}

namespace PJ
{
// Qt's upstream zh_CN catalogue predates the Qt 5 QPlatformTheme context.
// Extract the current standard-button keys into our supplementary catalogue.
[[maybe_unused]] static const char* const platform_button_texts[] = {
  QT_TRANSLATE_NOOP("QPlatformTheme", "OK"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Save"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Save All"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Open"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "&Yes"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Yes to &All"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "&No"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "N&o to All"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Abort"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Retry"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Ignore"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Close"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Cancel"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Discard"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Help"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Apply"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Reset"),
  QT_TRANSLATE_NOOP("QPlatformTheme", "Restore Defaults")
};

LanguageManager::LanguageManager(QString resource_root) : resource_root_(std::move(resource_root))
{
  initializeLanguageResources();
}

LanguageManager::~LanguageManager()
{
  QCoreApplication::removeTranslator(&app_translator_);
  QCoreApplication::removeTranslator(&qt_translator_);
}

QString LanguageManager::normalizePreference(const QString& preference)
{
  return preference == "en" || preference == "zh_CN" ? preference : QStringLiteral("system");
}

QString LanguageManager::savedPreference(const QSettings& settings)
{
  return normalizePreference(settings.value(kSettingsKey, "system").toString());
}

QString LanguageManager::resolveLanguage(const QString& preference, const QLocale& system_locale)
{
  const QString normalized = normalizePreference(preference);
  if (normalized != "system")
  {
    return normalized;
  }
  return system_locale.language() == QLocale::Chinese &&
                 system_locale.script() == QLocale::SimplifiedHanScript ?
             QStringLiteral("zh_CN") :
             QStringLiteral("en");
}

bool LanguageManager::install(const QString& preference, const QLocale& system_locale)
{
  QCoreApplication::removeTranslator(&app_translator_);
  QCoreApplication::removeTranslator(&qt_translator_);
  if (resolveLanguage(preference, system_locale) == "en")
  {
    return true;
  }
  if (!qt_translator_.load(resource_root_ + "/qt_zh_CN.qm") ||
      !app_translator_.load(resource_root_ + "/plotjuggler_zh_CN.qm"))
  {
    qWarning("Chinese translation resources could not be loaded; using English");
    return false;
  }
  if (!QCoreApplication::installTranslator(&qt_translator_) ||
      !QCoreApplication::installTranslator(&app_translator_))
  {
    QCoreApplication::removeTranslator(&app_translator_);
    QCoreApplication::removeTranslator(&qt_translator_);
    qWarning("Chinese translators could not be installed; using English");
    return false;
  }
  return true;
}
}  // namespace PJ
