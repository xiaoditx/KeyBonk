SHELL         := cmd
.SUFFIXES:

# ---------- 基本变量 ----------
CXX      := g++
WINDRES  := windres
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS  := -mwindows -municode
LDLIBS   := -luser32 -lgdi32 -lole32 -lgdiplus -lwinmm

# ---------- 目录变量 ----------
SRC_DIR  := src
INC_DIR  := include
RES_DIR  := resource
OBJ_DIR  := $(SRC_DIR)/obj
BIN      := KeyBonk.exe

# ---------- 源文件列表 ----------
CXX_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
RES_SRC  := $(RES_DIR)/resources.rc

# ---------- 自动推导对象 ----------
CXX_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CXX_SRCS))
RES_OBJ  := $(RES_DIR)/resources.o

# ---------- 默认目标 ----------
.PHONY: all clean
all: $(BIN)

# ---------- 链接 ----------
$(BIN): $(CXX_OBJS) $(RES_OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# ---------- 编译对象文件 ----------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

# ---------- 资源文件 ----------
$(RES_OBJ): $(RES_SRC)
	$(WINDRES) $< -F pe-x86-64 -o $@

# ---------- 自动依赖 ----------
-include $(CXX_OBJS:.o=.d)

# ---------- 目录与清理 ----------
$(OBJ_DIR):
	if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

clean:
	-del /Q $(OBJ_DIR)\*.o $(OBJ_DIR)\*.d $(RES_OBJ) $(BIN)

# ---------- 帮助 ----------
help:
	@echo 可用目标: all clean help
