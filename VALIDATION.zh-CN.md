# 中文版验证记录

日期：2026-10-09。基线：官方 3.17.2 / `034a5cc`。

## 已验证

- Windows x64 Release 完整构建成功：VS 2022 MSVC 19.44、Qt 5.15.2、CMake 3.31.6、Ninja 1.12.1。
- CTest 76/76 通过，包括语言解析、内嵌资源、保存/取消、CSV 解析和 ROS 字段测试。
- 应用词库 1029 条有效翻译全部完成，Qt 原生 `lupdate` 源键精确比对无缺项或旧项。
- 验证器 11 项变异测试通过；检查占位符、助记符、换行、HTML、链接和代码保留。
- NativeLanguageQA 使用 Windows 平台插件及应用实际使用的 Fusion 样式。
  浅色/深色、实际 DPR 1.0/1.5 下，语言下拉框、重启说明及确定/取消按钮显示中文。
  对复选框文字宽度、说明高度和控件重叠作了断言，并检查窗口截图。
- 已有 ULog Loader 动态插件打开本机真实 ULog：2680 条数值曲线、438903 个数据点。
  英文/中文分别加载后的全部曲线名、点数、时间和值 SHA256 相同：
  `3831c58a538e6d718794a7aca9b93b4c02e78f3120c6a2d907faa5352d30be89`。
  两种语言下均使用实际 `PlotWidgetBase`/Qwt 添加并绘制曲线。
- Python 3.12.14 标准库及 C 扩展在清理开发工具 PATH 后通过嵌入运行时 smoke test。

## 测试版范围和限制

本地构建包含 18 个官方插件 DLL，包括 CSV、MCAP、ULog、SerialPort、WebSocket、
Foxglove、ROS1/ROS2 消息解析、FFT、Lua 等。没有安装 ROS 运行环境。

未安装可选外部依赖，故没有编译 Parquet、MQTT、ZMQ、QtAV VideoViewer、
Mosaico、Protobuf 和 Zcm 插件。它们的仓库内 UI 已纳入词库，尚未进行运行验收。
nanobind `pj` 模块未启用；原生 PythonCustomFunction 已启用。

测试版 QtNetwork 缺少匹配的 OpenSSL 1.1 运行库，`supportsSsl()` 为 false。
离线 CSV/ULog 不受影响；HTTPS/WSS 功能未通过运行验收。
这是测试版的运行时依赖限制，不应当作完整联网发行版。

Qt 5 上游简体中文目录较旧：保留原始目录和许可说明，并在应用目录补充
QPlatformTheme 的 18 个标准按钮词条。操作系统原生文件对话框仍使用系统语言。
第三方插件、自定义皮肤及全部插件窗口的逐屏人工验证不在本次已验证范围内。

## 重现原生窗口与真实日志验证

设置 `PJ_QA_OUTPUT` 为截图目录，`PJ_QA_ULOG` 为本地日志完整路径，
`PJ_QA_ULOG_PLUGIN` 为所编译的 DataLoadULog.dll 路径，然后运行：

```powershell
$env:QT_QPA_PLATFORM = 'windows'
$env:PJ_QA_OUTPUT = "$PWD/build-zh/qa-images"
$env:PJ_QA_ULOG = 'C:/path/to/sample.ulg'
$env:PJ_QA_ULOG_PLUGIN = "$PWD/build-zh/bin/DataLoadULog.dll"
$env:QT_SCALE_FACTOR = '0.5'
$env:PJ_QA_EXPECT_DPR = '1'
./build-zh/bin/native_language_qa.exe
$env:QT_SCALE_FACTOR = '0.75'
$env:PJ_QA_EXPECT_DPR = '1.5'
./build-zh/bin/native_language_qa.exe preferencesAppearance
```

上述缩放系数用于本次基准 DPR=2 的 Windows 环境，其他显示器需调整系数。
测试会记录并检查真实 DPR，不能只凭环境变量声称达到目标缩放。
本地测试日志和截图保存在 `.cache`，用户飞行日志不上传仓库。
