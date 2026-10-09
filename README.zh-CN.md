# PlotJuggler 3.17.2 中文版

基于官方标签 `3.17.2`（`034a5cc919909aebb124e0dc1abbe2029c3eed0f`），
使用 Qt 5/C++17。支持跟随系统、English、简体中文。
默认开发分支为 `main-3.17.2-zh`；`main`、`main-4.x` 等官方同名分支保持与上游一致。

在“首选项 → 外观 → 语言”选择语言，点击“确定”保存，重新启动后生效。
点击“取消”不保存。默认跟随系统：简体中文系统使用中文，其余使用英文。
设置键为 `Preferences::language`，允许 `system`、`en`、`zh_CN`；无效值按跟随系统处理。

主窗口、绘图菜单、内置对话框和仓库内插件的可翻译界面纳入词库。
用户曲线名称、日志数据、路径、协议、代码示例及插件 ID 保持原样。
第三方插件需要提供自己的翻译。

## 实现

`LanguageManager` 在创建窗口前读取设置并安装两个 `QTranslator`，生命周期覆盖整个应用。
英文直接使用源文本；中文读取内嵌资源。加载失败会警告并回退英文。
应用词库另补齐 Qt 5 `QPlatformTheme` 标准按钮上下文。

```text
首选项语言选择 --确定--> QSettings
                           |
                        下次启动
                           v
系统语言 + 已保存选项 --> LanguageManager
                           |
                Qt 词库 + 应用词库（内嵌 .qm）
                           |
                     tr()/Designer UI
                           v
                  中文窗口、菜单和标准按钮

ULog/CSV --> 数据加载插件 --> PlotDataMapRef --> Qwt 绘图
                （数据处理流程不依赖语言选择）
```

## 构建和测试

Windows 验证工具链：VS 2022 x64、Qt 5.15.2 MSVC 2019 64 位、CMake、Ninja。
Qt 需要 Widgets、Svg、WebSockets、SerialPort、LinguistTools；测试还需要 Qt Test。
在 x64 Developer PowerShell 中执行，替换工具路径：

```powershell
$env:PATH = 'D:/Envir/Qt/5.15.2/msvc2019_64/bin;' + $env:PATH
cmake -S . -B build-zh -G Ninja -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=D:/Envir/Qt/5.15.2/msvc2019_64 `
  -DPJ_INSTALLATION=windows -DPJ_PLUGINS_DIRECTORY=bin `
  -DENABLE_LANGUAGE_TESTS=ON -DBUILD_TESTING=ON
cmake --build build-zh --parallel 6
ctest --test-dir build-zh --output-on-failure
```

GoogleTest 可选，安装后会额外编译现有 CSV/ROS 字段测试。
Python 数据处理功能需要匹配的 Python 开发头文件、导入库和运行时；
可用 `-DPython3_ROOT_DIR=...` 指定。部署时必须携带相同版本的 Python DLL 和标准库。

普通构建只通过 `lrelease` 生成 `.qm` 并嵌入可执行文件，不改写已提交的 `.ts`。
新增界面文字后显式执行 `cmake --build build-zh --target pj_update_translations`，
人工翻译新词条，再运行测试。维护规则见 [translations/README.md](translations/README.md)。

Windows 部署使用 `windeployqt --release --no-translations`，同时扫描应用及所编译的插件 DLL。
中文词库已经内嵌，不依赖外置语言包。还须部署 Wasmer、MSVC 和可选 Python 运行时。
本地测试版与现有安装独立，验证范围和已知限制见 [VALIDATION.zh-CN.md](VALIDATION.zh-CN.md)。

## 原版与后续维护

官方原版保留在 `3.17.2` 标签。中文开发基于该提交，不自动合并上游后续版本。
官方同名分支用于同步上游，中文默认分支单独维护。
完整上游说明见 [README.md](README.md)。
