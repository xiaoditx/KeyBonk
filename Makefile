# 设置代码页为UTF-8以支持中文路径和输出
$(shell chcp 65001 >nul)

# 编译架构选择，默认64位
ARCH ?= 64
DEBUG ?= -DKB_DEBUG

# 使用cmd为shell并禁止所有隐式规则
SHELL         := cmd
.SUFFIXES:

ifeq ($(ARCH),32)
CXX      = i686-w64-mingw32-g++
WINDRES  = windres
WINDRES_FLAG = -F pe-i386 -o
else
CXX      = g++
WINDRES  = windres
WINDRES_FLAG = -F pe-x86-64 -o
endif

# 基本变量 
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS  = -mwindows -municode
LDLIBS   = -luser32 -lgdi32 -lole32 -lgdiplus -lwinmm

# 目录变量
SRC_DIR   := src
INC_DIR   := include
RES_DIR   := resource
BUILD_BASE:= build
BUILD_DIR := $(BUILD_BASE)/$(ARCH)
OBJ_DIR   := $(BUILD_DIR)/obj
BIN       := $(BUILD_DIR)/KeyBonk.exe

# 源文件列表 
CXX_SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/windows/*.cpp) $(wildcard $(SRC_DIR)/hook/*.cpp) $(wildcard $(SRC_DIR)/functions/*.cpp)
RES_SRC  := $(RES_DIR)/resources.rc

# 自动推导对象 
CXX_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CXX_SRCS))
RES_OBJ  := $(OBJ_DIR)/rc/resources.o

# 默认目标（64位debug模式）
.PHONY: all clean help run release release64 release32 installer installer64 installer32
all: $(BIN)
	@echo 调试版本构建完成: $@

# 链接 
$(BIN): $(CXX_OBJS) $(RES_OBJ) | $(BUILD_DIR)/bin/default
	@echo 正在链接生成可执行文件 $@ ...
	@$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# 编译对象文件 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo 正在将源码文件 $< 编译到 $@
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@$(CXX) $(CXXFLAGS) $(DEBUG) -I$(INC_DIR) -I$(RES_DIR)  -MMD -MP -c $< -o $@

# 资源文件 
$(RES_OBJ): $(RES_SRC) ./include/globalDevelopmentControl.hpp | $(OBJ_DIR)/rc
	@echo 正在将RC文件 "$<" 编译到 $@ ...
	@$(WINDRES) $< $(WINDRES_FLAG) $@

# 自动依赖 
-include $(CXX_OBJS:.o=.d)

# 目录创建（对象文件目录和资源对象目录）
$(OBJ_DIR):
	@echo 正在创建对象文件目录 "$(OBJ_DIR)"
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(OBJ_DIR)/rc:
	@echo 正在创建资源对象目录 "$(OBJ_DIR)/rc"
	@if not exist "$(OBJ_DIR)/rc" mkdir "$(OBJ_DIR)/rc"

# 资源文件复制
$(BUILD_DIR)/bin/default:
	@echo 正在将资源文件复制到 "$@":
	@if not exist "$@" mkdir "$@"
	
	@echo == 正在将 "resource\audios" 复制到 "$@\audios\"
	@xcopy /E /Y "resource\audios" "$@\audios\\" >nul
	
	@echo == 正在将 "resource\background.png" 复制到 "$@"
	@xcopy /Y "resource\background.png" "$@" >nul

	@echo == 正在将 "resource\icon-org.png" 复制到 "$@"
	@xcopy /Y "resource\icon-org.png" "$@" >nul
	
	@echo 资源复制完毕

# 文件清理
clean:
	@echo 正在清理build目录 ...
	@if exist "$(BUILD_BASE)" rmdir /S /Q "$(BUILD_BASE)"
	@echo 清理完成
#	if exist "KeyBonk.exe" del "KeyBonk.exe"

# 帮助 
help:
	@type .\docs\makefileHelper.txt

run: $(BIN)
	@echo [运行] 运行 $(BIN)
	@$(BIN)

release: clean release64 release32
	@echo 所有发布版本构建完成

release64:
	@echo 正在构建64位发布版本 ...
	@$(MAKE) ARCH=64 all DEBUG=

release32:
	@echo 正在构建32位发布版本 ...
	@$(MAKE) ARCH=32 all DEBUG=

installer: clean release64 installer64 release32 installer32
	@echo 所有安装程序构建完成

installer64: installer.iss release64
	@echo 正在构建64位安装程序 ...
	@for /f "tokens=*" %%c in ('chcp') do set OLD_CODEPAGE=%%c
	@chcp 936 >nul
	@iscc /DMyAppArch="64" installer.iss
	@chcp 65001 >nul

installer32: installer.iss release32
	@echo 正在构建32位安装程序 ...
	@for /f "tokens=*" %%c in ('chcp') do set OLD_CODEPAGE=%%c
	@chcp 936 >nul
	@iscc /DMyAppArch="32" installer.iss
	@chcp 65001 >nul