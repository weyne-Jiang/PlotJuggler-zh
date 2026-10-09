# 中文版验证记录

日期：2026-10-09。基线：官方 3.17.2 / `034a5cc`。

## 已验证

- Windows x64 Release 完整构建成功：VS 2022 MSVC 19.44、Qt 5.15.2、CMake 3.31.6、Ninja 1.12.1。
- CTest 76/76 通过，包括语言解析、内嵌资源、保存/取消、CSV 解析和 ROS 字段测试。
- 应用词库 1052 条有效翻译全部完成，Qt 原生 `lupdate` 源键精确比对无缺项或旧项。
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

## 2026-10-09 Computer Use 补充检查

使用 Windows Computer Use（`@oai/sky`）直接打开独立测试目录中的 PlotJuggler，
通过鼠标、键盘、窗口截图及辅助功能树检查实际界面，未以源码扫描代替逐屏验收。

| 页面 | 本次检查结果 |
| --- | --- |
| 主窗口、应用/工具/帮助菜单 | 已检查中文；工具菜单四个插件名称补齐后已实际显示中文 |
| CSV 导出器、FFT、四元数转换、响应式 Lua 编辑器 | 已打开，参数、按钮和说明显示中文 |
| Lua 编辑器函数库页 | 控件中文；示例代码注释保留原文 |
| 首选项外观、行为、插件页 | 已打开，语言选择、设置说明和按钮中文 |
| 文件选择、CSV 导入表格及原始文本页 | 已打开，界面中文；测试数据名称保留原文 |
| CSV 日期时间格式帮助 | 已复查修复版，深色主题表格文字可读 |
| 绘图右键菜单、曲线编辑器、统计页 | 已打开，界面中文；测试曲线 A 的统计平均值为 5.5 |
| 数据处理器选择 | 修复版已实际打开，10 个名称全部中文；滑动平均、积分、方差/标准差参数及计算预览已抽查，内部 ID 与自动生成的曲线别名保留原文 |
| 自定义函数单函数、批量页 | 已打开，界面中文；函数库多选提示的错误 `notr` 标记已移除 |
| Serial、UDP、WebSocket 设置 | 已打开，连接参数、按钮中文；不提交连接 |
| WebSocket JSON 时间戳选项 | 修复版已实际打开并展开，复选框、字段名、示例提示中文；补充最小宽度避免窗口缩小时截断 |
| 速查表 | 主题和说明中文；嵌入的上游教程图片仍包含英文界面 |
| 关于、支持 | 最终版已实际复查，HTML 正文中文，支持页白底黑字清晰可读 |
| 颜色映射编辑器 | 已打开，说明、保存和关闭按钮中文；代码示例保留原文 |

用户将验收范围调整为“大致检查菜单即可”，并要求限时结束，本次按调整后的范围结束检查。
本次未完成 Foxglove/PlotJuggler Bridge、MCAP、色彩映射全部子页，以及所有
处理器参数页的逐屏检查。多次发生共享桌面输入打断或窗口捕获陈旧帧，
因此不能据此声称“所有页面均已通过中文验收”。上方原生 DPI/ULog 测试属于
此前验证记录，并非本次 Computer Use 重新验证。

保留英文的范围：插件注册标识、协议/编码标识、用户数据字段、Lua/Python 代码、
快捷键、日期格式表达式、外部链接、作者姓名与上游教程截图。
翻译仅用于显示，工具和处理器内部名称不变，未知第三方名称按原文回退。

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
