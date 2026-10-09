# 简体中文翻译维护

本项目以 PlotJuggler 3.17.2 的 Qt 5 源码为基础。应用目录中的
`plotjuggler_zh_CN.ts` 为第一方界面目录，使用 MPL-2.0；Qt 自带标准对话框
目录单独保存于应用 translations/qt，许可证和上游出处见该目录。

在项目根目录执行（Python 3，Qt 5 Linguist 工具）：

```powershell
python scripts/update_translations.py --lupdate C:/Qt/5.15.2/msvc2019_64/bin/lupdate.exe
python scripts/validate_translations.py --lupdate C:/Qt/5.15.2/msvc2019_64/bin/lupdate.exe
python scripts/test_validate_translations.py
```

必须使用与构建匹配的 Qt 5 lupdate/lrelease。新增 tr() 后重新提取、人工翻译、
验证再构建。更新脚本使用真实 Qt C++/Designer 解析器；验证脚本可从空目录
重新提取并比较完整键（上下文、原文、消歧注释、复数标志），拒绝漏项和旧键。
MainWindow 的实际上下文为 MainWindow；不可替换为 PJ::MainWindow。

提取范围为 plotjuggler_app、plotjuggler_base、plotjuggler_plugins 的生产
cpp/cc/cxx/h/hpp/ui 文件。排除任意层级的 3rdparty、rosx_introspection、
translations、test/tests、demo/demos 和 build* 目录。测试与数据样本不参与。
Designer 的数值默认值、日期格式令牌、协议标识、URI、代码、路径、快捷键、
空富文本及运行时覆盖的占位标签使用 notr=true。插件 ID、协议元数据、
用户数据和可执行代码不可翻译。品牌/序列化名称及少量运行时数值片段的
明确不变量列于 validate_translations.py；禁止用宽泛英文白名单掩盖漏译。

验证涵盖完成状态、中文正文、重复键、过时项、占位符的重复次数
（包括 %0、%L1、%n）、中文单复数形式、HTML 标签及属性、链接/URL、
code 示例、换行、助记符和 &&。富文本中的 Lua 示例、变量、路径、CSS 和
品牌保留原样；中文翻译只改变说明性正文。

Qt 5 原生平台标准按钮实际使用 QPlatformTheme 上下文；第一方目录补充了
18 条标准控件文本以覆盖旧 Qt 官方目录中缺失的该上下文。

术语统一表见 glossary.md。无需联网生成翻译；所有构建输入均在新仓库内。
