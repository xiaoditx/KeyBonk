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
	@echo Build is done

# 链接 
$(BIN): $(CXX_OBJS) $(RES_OBJ) | $(BUILD_DIR)/bin/default
	@echo Linking ...
	@$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# 编译对象文件 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo Compile $< into $@
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@$(CXX) $(CXXFLAGS) $(DEBUG) -I$(INC_DIR) -I$(RES_DIR)  -MMD -MP -c $< -o $@

# 资源文件 
$(RES_OBJ): $(RES_SRC) ./include/globalDevelopmentControl.hpp | $(OBJ_DIR)/rc
	@echo Compile rc file "$<" into $@
	@$(WINDRES) $< $(WINDRES_FLAG) $@

# 自动依赖 
-include $(CXX_OBJS:.o=.d)

# 目录创建（对象文件目录和资源对象目录）
$(OBJ_DIR):
	@echo Making the build folder ...
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(OBJ_DIR)/rc:
	@echo Making the rc folder in $(OBJ_DIR)
	@if not exist "$(OBJ_DIR)/rc" mkdir "$(OBJ_DIR)/rc"

# 资源文件复制
$(BUILD_DIR)/bin/default:
	@echo Copy resources to "$@":
	@if not exist "$@" mkdir "$@"
	
	@echo == Copy "resource\audios" to "$@\audios\"
	@xcopy /E /Y "resource\audios" "$@\audios\\" >nul
	
	@echo == Copy "resource\background.png" to "$@"
	@xcopy /Y "resource\background.png" "$@" >nul

	@echo == Copy "resource\icon-org.png" to "$@"
	@xcopy /Y "resource\icon-org.png" "$@" >nul
	
	@echo Resources copy was done

# 文件清理
clean:
	@echo The build folder has been deleted
	@if exist "$(BUILD_BASE)" rmdir /S /Q "$(BUILD_BASE)"
#	if exist "KeyBonk.exe" del "KeyBonk.exe"

# 帮助 
help:
	@echo Available targets: all clean help run release release64 release32 installer installer64 installer32
	@echo Default build: 64-bit debug mode
	@echo 	- all: Build the project in default mode
	@echo 	- clean: Remove all build files
	@echo 	- help: Show this help message
	@echo 	- run: Run the built executable
	@echo 	- release: Build both 64-bit and 32-bit release versions
	@echo 	- release64: Build 64-bit release version
	@echo 	- release32: Build 32-bit release version
	@echo 	- installer: Build installers for both 64-bit and 32-bit
	@echo 	- installer64: Build installer for 64-bit release version
	@echo 	- installer32: Build installer for 32-bit release version

run: $(BIN)
	@echo [RUN] Running $(BIN)
	@$(BIN)

release: clean release64 release32
	@echo All release builds are done

release64:
	@echo Building 64-bit release ...
	@$(MAKE) ARCH=64 all DEBUG=

release32:
	@echo Building 32-bit release ...
	@$(MAKE) ARCH=32 all DEBUG=

installer: clean release64 installer64 release32 installer32
	@echo All installer builds are done

installer64: installer.iss release64
	@echo Building installer ...
	@iscc /DMyAppArch="64" installer.iss

installer32: installer.iss release32
	@echo Building installer ...
	@iscc /DMyAppArch="32" installer.iss