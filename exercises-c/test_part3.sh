#!/bin/bash

# 定义颜色，方便查看结果
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# 定义基础目录（脚本应当在 exercises-c 目录下运行）
BASE_DIR=$(pwd)

# 初始化计数器
PASS_COUNT=0
TOTAL_COUNT=0
FAIL_LIST=()

echo "=========================================="
echo "    Starting CHERI Part 3 Auto-Test       "
echo "=========================================="
echo "Looking for specific error messages in output..."
echo ""

# 定义一个函数来测试单个目录
test_exercise() {
    local dir_name=$1
    local search_pattern=$2
    
    ((TOTAL_COUNT++))
    
    # 检查目录是否存在
    if [ ! -d "$dir_name" ]; then
        echo -e "[${RED}SKIP${NC}] Directory $dir_name not found."
        return
    fi

    echo -n "Testing $dir_name ... "

    # 进入目录
    cd "$dir_name" || return

    xmake f --debug_run=false > /dev/null 2>&1 # 运行 xmake run 并捕获所有输出 (stdout 和 stderr)
    # 使用 timeout 防止程序死锁，虽然 CHERI 通常会终止，但为了保险
    output=$(timeout 15s xmake run 2>&1)
    
    # 检查输出中是否包含我们期待的错误处理日志
    if echo "$output" | grep -Fq "$search_pattern"; then
        echo -e "[${GREEN}PASS${NC}]"
        ((PASS_COUNT++))
    else
        echo -e "[${RED}FAIL${NC}]"
        FAIL_LIST+=("$dir_name")
        # 如果失败，打印最后几行日志帮助调试
        echo "    >>> Failure Log Snippet:"
        echo "$output" | tail -n 5 | sed 's/^/        /'
        echo "    >>> Expected: '$search_pattern'"
    fi

    # 返回上一级目录
    cd "$BASE_DIR" || return
}

# ==========================================
# 开始按顺序测试
# 对应我们在代码中写入的 CHERIOT_DEBUG_LOG 内容
# ==========================================

test_exercise "01.heap-buffer-over-read"   "Error detected: Heap buffer over-read"
test_exercise "02.heap-buffer-over-write"  "Error detected: Heap buffer over-write"
test_exercise "03.heap-buffer-under-write" "Error detected: Heap buffer under-write"
test_exercise "04.stack-buffer-over-write" "Error detected: Stack buffer over-write"
test_exercise "05.oob-pointer-arithmetic"  "Error detected: OOB Pointer Arithmetic"
test_exercise "06.use-after-free"          "Error detected: Use After Free"
test_exercise "07.double-free"             "Error detected: Double Free"
test_exercise "08.type-confusion"          "Error detected: Type confusion"

echo ""
echo "=========================================="
echo "    Test Summary                          "
echo "=========================================="
if [ ${#FAIL_LIST[@]} -eq 0 ]; then
    echo -e "${GREEN}All $TOTAL_COUNT exercises PASSED!${NC}"
    echo "Great job! Your Part 3 implementation looks correct."
else
    echo -e "${RED}$PASS_COUNT / $TOTAL_COUNT passed.${NC}"
    echo "The following exercises failed:"
    for item in "${FAIL_LIST[@]}"; do
        echo "  - $item"
    done
    echo ""
    echo "Tip: Check if you saved the files and if the debug string matches exactly."
fi
