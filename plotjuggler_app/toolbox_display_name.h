/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef PJ_TOOLBOX_DISPLAY_NAME_H
#define PJ_TOOLBOX_DISPLAY_NAME_H

#include <QCoreApplication>
#include <QString>

namespace PJ
{
// Plugin names remain stable identifiers in layouts and the plugin manager.
// Only the application menu uses these translated display labels.
inline QString toolboxDisplayName(const QString& plugin_name)
{
  static const char* const names[] = { QT_TRANSLATE_NOOP("ToolboxMenu", "CSV/Parquet Exporter"),
                                       QT_TRANSLATE_NOOP("ToolboxMenu", "Fast Fourier Transform"),
                                       QT_TRANSLATE_NOOP("ToolboxMenu", "Quaternion to RPY"),
                                       QT_TRANSLATE_NOOP("ToolboxMenu", "Reactive Script Editor") };
  for (const auto* name : names)
  {
    if (plugin_name == QLatin1String(name))
    {
      return QCoreApplication::translate("ToolboxMenu", name);
    }
  }
  return plugin_name;
}
}  // namespace PJ

#endif
