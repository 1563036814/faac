#!/bin/bash
# 编译选项
SRC_FILE="faac_exampl.c"
OUTPUT_FILE="faac_example"
INCLUDE_DIR="../faac/include"
LIB_FILE="../faac/libfaac/.libs/libfaac.a"

# 检查源文件是否存在
if [ ! -f "$SRC_FILE" ]; then
    echo "Error: 源文件 $SRC_FILE 不存在"
    exit 1
fi

# 检查头文件目录是否存在
if [ ! -d "$INCLUDE_DIR" ]; then
    echo "Error: 头文件目录 $INCLUDE_DIR 不存在"
    exit 1
fi

# 检查库文件是否存在
if [ ! -f "$LIB_FILE" ]; then
    echo "Error: 库文件 $LIB_FILE 不存在"
    exit 1
fi

# 编译
echo "编译 $SRC_FILE..."
gcc -I"$INCLUDE_DIR" -o "$OUTPUT_FILE" "$SRC_FILE" "$LIB_FILE" -lm

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "✅ 编译成功: $OUTPUT_FILE"
else
    echo "❌ 编译失败"
    exit 1
fi