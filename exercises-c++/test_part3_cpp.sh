#!/bin/bash

# 定义颜色
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# 定义基础目录（脚本应当在 exercises-c++ 目录下运行）
BASE_DIR=$(pwd)

PASS_COUNT=0
TOTAL_COUNT=0
FAIL_LIST=()

echo "=========================================="
echo "  Starting CHERI Part 3 Auto-Test (C++)   "
echo "=========================================="
echo "Looking for specific error messages in output..."
echo ""

# 测试函数
test_exercise() {
    local dir_name=$1
    local search_pattern=$2
    
    ((TOTAL_COUNT++))
    
    if [ ! -d "$dir_name" ]; then
        echo -e "[${RED}SKIP${NC}] Directory $dir_name not found."
        return
    fi

    echo -n "Testing $dir_name ... "

    cd "$dir_name" || return

    # -----------------------------------------------
    # 强制关闭 debug_run 模式，防止脚本卡死
    # -----------------------------------------------
    xmake f --debug_run=false > /dev/null 2>&1
    
    # 运行 xmake run
    output=$(timeout 15s xmake run 2>&1)
    
    # 检查输出
    if echo "$output" | grep -Fq "$search_pattern"; then
        echo -e "[${GREEN}PASS${NC}]"
        ((PASS_COUNT++))
    else
        echo -e "[${RED}FAIL${NC}]"
        FAIL_LIST+=("$dir_name")
        echo "    >>> Failure Log Snippet:"
        echo "$output" | tail -n 10 | sed 's/^/        /'
        echo "    >>> Expected: '$search_pattern'"
    fi

    cd "$BASE_DIR" || return
}

# ==========================================
# C++ 目录列表及期待的错误信息
# ==========================================

test_exercise "01.heap-buffer-over-read"   "Error detected: Heap buffer over-read (C++)"
test_exercise "02.heap-buffer-over-write"  "Error detected: Heap buffer over-write (C++)"
test_exercise "03.heap-buffer-under-write" "Error detected: Heap buffer under-write (C++)"
test_exercise "04.stack-buffer-over-write" "Error detected: Stack buffer over-write (C++)"
test_exercise "05.oob-pointer-arithmetic"  "Error detected: OOB Pointer Arithmetic (C++)"
test_exercise "06.use-after-free"          "Error detected: Use After Free (C++)"
test_exercise "07.double-free"             "Error detected: Double Free (accessing revoked memory C++)"
test_exercise "08.type-confusion"          "Error detected: Type confusion (C++)"

echo ""
echo "=========================================="
echo "    Test Summary (C++)                    "
echo "=========================================="
if [ ${#FAIL_LIST[@]} -eq 0 ]; then
    echo -e "${GREEN}All $TOTAL_COUNT C++ exercises PASSED!${NC}"
    echo "Excellent work! Part 3 is fully complete."
else
    echo -e "${RED}$PASS_COUNT / $TOTAL_COUNT passed.${NC}"
    echo "The following exercises failed:"
    for item in "${FAIL_LIST[@]}"; do
        echo "  - $item"
    done
    echo ""
    echo "Tip: Make sure you removed '#include <fail-simulator-on-error.h>' from the .cc files!"
fi
