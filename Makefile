# 编译架构选择，默认64位
ARCH ?= 64

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
BUILD_BASE:= build
BUILD_DIR := $(BUILD_BASE)/$(ARCH)
SRC_DIR   := src
INC_DIR   := include
RES_DIR   := resource
OBJ_DIR   := $(BUILD_DIR)/obj
BIN       := $(BUILD_DIR)/KeyBonk.exe

# 源文件列表 
CXX_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
RES_SRC  := $(RES_DIR)/resources.rc

# 自动推导对象 
CXX_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CXX_SRCS))
RES_OBJ  := $(OBJ_DIR)/src/resources.o

# 默认目标 
.PHONY: all clean help
all: $(BIN)

# 链接 
$(BIN): $(CXX_OBJS) $(RES_OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# 编译对象文件 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

# 资源文件 
$(RES_OBJ): $(RES_SRC)
	$(WINDRES) $< $(WINDRES_FLAG) $@

# 自动依赖 
-include $(CXX_OBJS:.o=.d)

# 目录创建和文件复制
$(OBJ_DIR):
	if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	if not exist "$(OBJ_DIR)\src" mkdir "$(OBJ_DIR)\src"
	if not exist "$(BUILD_DIR)\resource" mkdir "$(BUILD_DIR)\resource"
	xcopy /E /Y "resource\audios" "$(BUILD_DIR)\resource\audios\\" >nul
	xcopy /Y "resource\background.png" "$(BUILD_DIR)\resource\\" >nul
	xcopy /Y "resource\icon-org.png" "$(BUILD_DIR)\resource\\" >nul
	echo Resources copied to $(BUILD_DIR)\resource

# 文件清理
clean:
	if exist "$(BUILD_BASE)" rmdir /S /Q "$(BUILD_BASE)"
#	if exist "KeyBonk.exe" del "KeyBonk.exe"

# 帮助 
help:
	@echo 可用目标: all clean help