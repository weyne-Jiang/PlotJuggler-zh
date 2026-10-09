/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef PJ_TRANSFORM_DISPLAY_NAME_H
#define PJ_TRANSFORM_DISPLAY_NAME_H

#include <QCoreApplication>
#include <QString>

namespace PJ
{
inline QString transformDisplayName(const QString& identifier)
{
  static const char* const names[] = {
    QT_TRANSLATE_NOOP("TransformMenu", "Absolute"),
    QT_TRANSLATE_NOOP("TransformMenu", "Binary Filter"),
    QT_TRANSLATE_NOOP("TransformMenu", "Derivative"),
    QT_TRANSLATE_NOOP("TransformMenu", "Integral"),
    QT_TRANSLATE_NOOP("TransformMenu", "Moving Average"),
    QT_TRANSLATE_NOOP("TransformMenu", "Moving Root Mean Squared"),
    QT_TRANSLATE_NOOP("TransformMenu", "Moving Variance / Stdev"),
    QT_TRANSLATE_NOOP("TransformMenu", "Outlier Removal"),
    QT_TRANSLATE_NOOP("TransformMenu", "Samples Counter"),
    QT_TRANSLATE_NOOP("TransformMenu", "Scale/Offset")
  };
  for (const auto* name : names)
  {
    if (identifier == QLatin1String(name))
    {
      return QCoreApplication::translate("TransformMenu", name);
    }
  }
  return identifier;
}
}  // namespace PJ

#endif
