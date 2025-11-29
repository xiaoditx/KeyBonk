```
master
│  CONTRIBUTING.md：贡献指南
│  installer.exe：安装程序
│  installer.iss：安装脚本
│  KeyBonk.exe：主程序
│  LICENSE：WTFPL协议
│  Makefile：Makefile文件
│  make_exe.bat：编译脚本
│  README.md：项目介绍
│
├─.vscode：VSCode设置文件
│      settings.json：VSCode设置
│
├─docs：文档文件
│      files：项目文件
│
├─include：头文件
│      keyboard_hook.h：键盘钩子
│      keybonk_global.h：全局变量
│      utils.h：工具函数
│      window_manager.h：窗口管理
│
├─resource：资源文件
│  │  background.png：背景图片
│  │  icon-org.png：原始图标
│  │  icon.png：图标
│  │  resources.h：资源头文件
│  │  resources.o：资源对象文件
│  │  resources.rc：资源脚本
│  │
│  ├─audios：音频文件
│  │      74.mp3
│  │      74.wav
│  │      77.mp3
│  │      77.wav
│  │      78.mp3
│  │      78.wav
│  │      84.mp3
│  │      84.wav
│  │
│  └─ico：图标文件
│          64.ico：64x64图标
│
├─src：源文件
│  │  global.cpp：全局变量
│  │  keyboard_hook.cpp：键盘钩子
│  │  main.cpp：主程序
│  │  utils.cpp：工具函数
│  │  window_manager.cpp：窗口管理
│  │
│  └─obj：对象文件
│          global.o：全局变量对象文件
│          keyboard_hook.o：键盘钩子对象文件
│          main.o：主程序对象文件
│          utils.o：工具函数对象文件
│          window_manager.o：窗口管理对象文件
│
└─test：测试文件
        test.cpp：测试程序
```