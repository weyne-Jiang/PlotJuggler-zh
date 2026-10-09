# PlotJuggler 3.17.2 中文版

基于官方 **3.17.2**，支持 **跟随系统 / English / 简体中文**，语言设置保存后重启生效。
本仓库的默认开发分支为 `main-3.17.2-zh`；`main`、`main-4.x` 等官方同名分支保持与上游一致。

![PlotJuggler](docs/plotjuggler3_banner.svg)

## 下载和安装

前往 [Releases](https://github.com/weyne-Jiang/PlotJuggler-zh/releases) 下载
`PlotJuggler-3.17.2-zh.2-Windows-x64.exe`。
首版为 Windows x64 离线预发布安装包，安装后无需另外安装 Qt 或 Python。
安装包及其 SHA256 校验文件一起提供。

默认安装目录：`%LOCALAPPDATA%/PlotJuggler-3.17.2-zh`。
开始菜单和桌面快捷方式名称为“PlotJuggler 3.17.2 中文版”。
卸载使用安装目录下的 `maintenancetool.exe`。

启动后，在 **首选项 → 外观 → 语言** 中选择：

- **跟随系统**：简体中文系统使用中文，其他系统使用英文。
- **English**：始终使用英文。
- **简体中文**：始终使用中文。

点击“确定”保存，然后重新启动；点击“取消”不保存。
中文词库内嵌在程序中，不需要下载外置汉化补丁。

## 功能和验证

- 1029 条中文词条，覆盖主界面、菜单、首选项、绘图、内置对话框和仓库内插件界面。
- 用户数据、曲线名、路径、协议及代码示例保持原样。
- 76 项自动化测试通过；首选项在明暗主题及实际 100%/150% DPI 下通过验证。
- 真实 ULog 在英文和中文模式下读取结果一致，2680 条数值曲线、438903 个数据点。
- Windows 包包含 18 个内置插件，包括 CSV、MCAP、ULog、SerialPort、WebSocket、FFT 和 Lua。

## 首版范围

本安装包主要验证离线 CSV/ULog 和中文界面。缺少匹配的 OpenSSL 运行库，
HTTPS/WSS 尚不可用。Parquet、MQTT、ZMQ、QtAV VideoViewer、Mosaico、Protobuf、
Zcm 等可选插件未构建；ROS1/ROS2 消息解析插件可用，但未附带 ROS 运行环境。
第三方插件需要自行提供翻译。详细限制见 [验证记录](VALIDATION.zh-CN.md)。

## 开发与维护

- [中文使用、实现原理与构建说明](README.zh-CN.md)
- [翻译维护规范](translations/README.md) · [术语表](translations/glossary.md)
- [验证记录](VALIDATION.zh-CN.md)
- [官方原版项目说明](README.upstream.md)
- [官方项目](https://github.com/PlotJuggler/PlotJuggler)

中文分支基线为官方 `3.17.2`（`034a5cc`）；后续中文改进在 `main-3.17.2-zh` 开发。
上游说明中的功能和下载链接属于官方项目，中文安装包以本仓库 Release 为准。

## 许可证

应用和中文翻译采用 [MPL-2.0](LICENSE.md)。Qt、Python、Wasmer 等依赖的许可证
随安装包提供；上游贡献者、致谢和赞助信息保留在 [官方原版说明](README.upstream.md)。
