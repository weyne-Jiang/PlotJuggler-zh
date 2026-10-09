/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QCryptographicHash>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QPluginLoader>
#include <QPushButton>
#include <QSettings>
#include <QStyleFactory>
#include <QTemporaryDir>
#include <QtTest>
#include <algorithm>
#include <cstring>
#include "language_manager.h"
#include "preferences_dialog.h"
#include "stylesheet.h"
#include "PlotJuggler/dataloader_base.h"
#include "PlotJuggler/plotwidget_base.h"

class NativeLanguageQA : public QObject
{
  Q_OBJECT
private slots:
  void preferencesAppearance()
  {
    QTemporaryDir settings;
    QVERIFY(settings.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settings.path());
    QCoreApplication::setOrganizationName("PlotJugglerLanguageQA");
    QCoreApplication::setApplicationName("Native");
    PJ::LanguageManager manager;
    QVERIFY(manager.install("zh_CN"));
    const QString output = qEnvironmentVariable("PJ_QA_OUTPUT");
    QVERIFY(QDir().mkpath(output));
    for (const QString& theme : { QString("light"), QString("dark") })
    {
      QFile stylesheet(":/resources/stylesheet_" + theme + ".qss");
      QVERIFY(stylesheet.open(QIODevice::ReadOnly));
      SetApplicationStyleSheet(QString::fromUtf8(stylesheet.readAll()));
      PreferencesDialog dialog;
      dialog.show();
      QVERIFY(QTest::qWaitForWindowExposed(&dialog));
      auto* combo = dialog.findChild<QComboBox*>("comboBoxLanguage");
      auto* hint = dialog.findChild<QLabel*>("labelLanguageRestart");
      auto* buttons = dialog.findChild<QDialogButtonBox*>("buttonBox");
      QVERIFY(combo && hint && buttons);
      combo->setCurrentIndex(combo->findData("zh_CN"));
      QTest::qWait(100);
      const qreal dpr = dialog.devicePixelRatioF();
      qInfo() << "Theme" << theme << "actual DPR" << dpr << "size" << dialog.size();
      const double expected = qEnvironmentVariable("PJ_QA_EXPECT_DPR").toDouble();
      QVERIFY(qAbs(dpr - expected) < 0.05);
      QVERIFY(!hint->isHidden());
      QCOMPARE(hint->text(), QString::fromUtf8("重新启动 PlotJuggler 后生效。"));
      qInfo() << "Native Cancel text" << buttons->button(QDialogButtonBox::Cancel)->text();
      QVERIFY(
          buttons->button(QDialogButtonBox::Cancel)->text().contains(QString::fromUtf8("取消")));
      QVERIFY(!combo->geometry().intersects(hint->geometry()));
      QVERIFY(combo->width() >=
              combo->fontMetrics().horizontalAdvance(QString::fromUtf8("简体中文")) + 30);
      auto* separator = dialog.findChild<QCheckBox*>("checkBoxSeparator");
      QVERIFY(separator);
      QVERIFY(separator->width() >= separator->sizeHint().width());
      QVERIFY(hint->height() >= hint->heightForWidth(hint->width()));
      QVERIFY(dialog.grab().save(output + "/preferences_" + theme + "_" + QString::number(dpr) +
                                 ".png"));
    }
  }

  void ulogLanguageInvariant()
  {
    const QString input = qEnvironmentVariable("PJ_QA_ULOG");
    if (input.isEmpty())
    {
      QSKIP("Set PJ_QA_ULOG to a real ULog fixture");
    }
    QPluginLoader plugin(qEnvironmentVariable("PJ_QA_ULOG_PLUGIN"));
    auto* loader = qobject_cast<PJ::DataLoader*>(plugin.instance());
    QVERIFY2(loader, qPrintable(plugin.errorString()));
    PJ::LanguageManager manager;
    QByteArray previous;
    for (const QString& language : { QString("en"), QString("zh_CN") })
    {
      QVERIFY(manager.install(language));
      PJ::FileLoadInfo info;
      info.filename = input;
      PJ::PlotDataMapRef data;
      QVERIFY(loader->readDataFromFile(&info, data));
      QVERIFY(!data.numeric.empty());
      std::vector<std::string> names;
      for (const auto& entry : data.numeric)
      {
        names.push_back(entry.first);
      }
      std::sort(names.begin(), names.end());
      QCryptographicHash hash(QCryptographicHash::Sha256);
      size_t points = 0;
      PJ::PlotData* curve_data = nullptr;
      std::string curve_name;
      for (const auto& name : names)
      {
        auto& series = data.numeric.at(name);
        hash.addData(name.c_str(), int(name.size()));
        const quint64 count = series.size();
        hash.addData(reinterpret_cast<const char*>(&count), sizeof(count));
        for (size_t i = 0; i < series.size(); ++i)
        {
          const auto& point = series.at(i);
          hash.addData(reinterpret_cast<const char*>(&point.x), sizeof(point.x));
          hash.addData(reinterpret_cast<const char*>(&point.y), sizeof(point.y));
        }
        points += series.size();
        if (!curve_data && series.size() > 100)
        {
          curve_data = &series;
          curve_name = name;
        }
      }
      const QByteArray digest = hash.result();
      qInfo() << language << "series" << data.numeric.size() << "points" << points << "SHA256"
              << digest.toHex();
      if (!previous.isEmpty())
      {
        QCOMPARE(digest, previous);
      }
      previous = digest;
      QVERIFY(curve_data);
      PJ::PlotWidgetBase plot(nullptr);
      plot.resize(800, 450);
      QVERIFY(plot.addCurve(curve_name, *curve_data));
      plot.resetZoom();
      plot.show();
      QVERIFY(QTest::qWaitForWindowExposed(&plot));
      QTest::qWait(100);
      QVERIFY(
          plot.grab().save(qEnvironmentVariable("PJ_QA_OUTPUT") + "/ulog_" + language + ".png"));
      for (QWidget* widget : qApp->topLevelWidgets())
      {
        if (widget != &plot)
        {
          widget->close();
        }
      }
      QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    }
  }
};

int main(int argc, char** argv)
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QApplication app(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));
  NativeLanguageQA qa;
  return QTest::qExec(&qa, argc, argv);
}
#include "native_language_qa.moc"
