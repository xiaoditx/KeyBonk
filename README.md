![KeyBonk](./resource/icon.png)

[贡献者帮助信息](#开发信息)

# 发声键盘-KeyBonk

> [!note]
> 当前版本为预先发布版，版本号V1.0.0.0
>
> 当前代码错误处理功能不足


基于曾经做过的唯一能入眼的软件“坤音键盘”的4.0版本重构的C++版本，原版公开了源码但没有在GitHub上开源，因为那是一个易语言项目，源码只有一个文件所以似乎没有什么开到GitHub的必要（况且GitHub默认不识别易语言，虽然我们可以手动设定但我不喜欢）

### 重构的变动

重构将语言换为了C++，使得运行效率得到了提升，同时抛弃了原本使用注册热键实现的极为愚蠢的方式，改用了键盘钩子作为了监视方法。

这是我接触Win32开发的第一个项目，算是我练手用的了，希望各位可以喜欢

### 安装

软件安装包可以在[Release](https://github.com/xiaoditx/KeyBonk/releases)页面找到，早期版本为单文件

软件仅支持Windows，当前仅提供64位版本（32位存在但未经测试所有不提供），可用自行下载后进行编译

### 使用说明

打开软件，软件将监测按键，默认检测`j`、`n`、`t`、`m`四个按键，按下播放对应音频

音频文件在`./resource/audios`下，文件名为想要监测的按键的虚拟键值+“.wav”（仅支持wav）。有关虚拟键值，想要获取可以使用我在BeepMusic中使用过的工具：[键值查询工具](https://github.com/xiaoditx/BeepMusic/tree/main/tools/XD%E5%BC%80%E5%8F%91%E5%B7%A5%E5%85%B7-T1)。也可以查阅[微软文档](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes)

背景文件可更换，使用自己的图片替换掉`./resource/background.png`即可，目前测试仅证明了png有效。当前版本不支持自动检测图片尺寸，不支持颜色过滤自定义，这些功能将在V2甚至V3支持

### 开发信息

软件技术栈

- win32
- C++17
- GDI+
- COM库

开发环境

- Windows10-x64
- MSYS2（MingGW64）
- VScode
- GNU Make