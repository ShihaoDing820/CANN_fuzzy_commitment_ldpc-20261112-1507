# CANN Fuzzy Commitment LDPC 算子原型

这是一个基于 CANN 9.0.0 和 Ascend C 的 Fuzzy Commitment + QC-LDPC 可运行原型。项目使用 CANN CPU 孪生调试模式，因此即使没有 Ascend NPU 和驱动，也可以验证算子的编译、启动和计算流程。

当前实现包括：

- 固定的 `(3, 6)` 正则 QC-LDPC 校验图；
- 参数 `N=96`、`K=50`、`M=48`、边数 `288`；
- Fuzzy Commitment 辅助数据 `helper = witness XOR codeword`；
- Ascend C Kernel 中的异或恢复、LLR 初始化、Normalized Min-Sum 译码和 syndrome 校验；
- Host 侧 SHA-256 和常数时间摘要比较；
- 10 组确定性端到端测试；
- Host 参考译码器与 CANN 输出逐位比对。

## 运行环境

推荐环境：

- Linux x86_64；
- CANN Toolkit 9.0.0；
- CMake 3.16 或更高版本；
- 支持 C++17 的编译器；
- OpenSSL 开发库；
- Bash。

本项目默认编译目标为 `dav-2201`，对应 Atlas A2/A3 系列使用的 Ascend C 架构。CPU 孪生测试不要求安装 NPU 驱动。

本项目在以下环境验证通过：

```text
CANN Toolkit: 9.0.0
Python:        3.11.18
CMake:         3.28.3
Architecture:  dav-2201
```

## 获取代码

```bash
git clone https://github.com/ShihaoDing820/CANN_fuzzy_commitment_ldpc-20261112-1507.git
cd CANN_fuzzy_commitment_ldpc-20261112-1507
```

## 一键构建和测试

如果使用本项目最初配置的虚拟机环境，直接执行：

```bash
./run-tests.sh
```

脚本会完成以下操作：

1. 激活 CANN 环境；
2. 清除旧的 CMake 配置缓存；
3. 使用 CPU 孪生模式配置工程；
4. 使用 Bisheng 编译 Ascend C Kernel；
5. 链接 Host 测试程序；
6. 生成并运行 10 组测试数据。

## 在其他 CANN 环境运行

如果 CANN 环境脚本不在默认路径，通过 `CANN_ACTIVATE_SCRIPT` 指定：

```bash
CANN_ACTIVATE_SCRIPT=/path/to/cann/set_env.sh ./run-tests.sh
```

也可以先手动激活环境：

```bash
source /path/to/cann/set_env.sh
./run-tests.sh
```

脚本按以下顺序寻找环境：

1. `CANN_ACTIVATE_SCRIPT` 指定的脚本；
2. `/home/dsh/cann-dev/activate.sh`；
3. `$HOME/Ascend/cann/set_env.sh`；
4. `/usr/local/Ascend/ascend-toolkit/set_env.sh`；
5. 当前 Shell 中已经设置的 `ASCEND_HOME_PATH`。

## 手动构建

```bash
source /path/to/cann/set_env.sh

cmake --fresh \
  -S . \
  -B build \
  -DCMAKE_ASC_RUN_MODE=cpu \
  -DCMAKE_ASC_ARCHITECTURES=dav-2201

cmake --build build --parallel 2
./build/fuzzy_ldpc_test
```

## 预期输出

成功时可以看到 10 个 CPU 孪生 Core 正常退出：

```text
[SUCCESS][CORE_0] exit success!
...
[SUCCESS][CORE_9] exit success!
```

每组测试还会输出样本类型、比特错误数、syndrome、迭代次数、认证结果以及与 Host 参考实现的比对结果：

```text
[PASS][CASE_00] type=legitimate bit_errors=0 syndrome=0 iterations=1 authenticated=yes reference_match=yes
...
[PASS][CASE_08] type=impostor bit_errors=51 syndrome=7 iterations=30 authenticated=no reference_match=yes
[PASS][CASE_09] type=impostor bit_errors=55 syndrome=10 iterations=30 authenticated=no reference_match=yes
[PASS] Fuzzy commitment CANN test passed 10/10 cases.
```

10 组数据包括：

- 8 组合法样本，包含 0～3 位随机错误，预期成功恢复并通过 SHA-256；
- 2 组冒充样本，包含大量比特差异，预期认证失败；
- 所有 CANN 输出均与 Host 参考译码器逐位比较。

## 算法流程

注册阶段：

```text
随机消息 -> QC-LDPC 编码 -> codeword
witness XOR codeword     -> helper
SHA-256(codeword)        -> commitment tag
```

验证阶段：

```text
new_witness XOR helper
        -> LLR 初始化
        -> Normalized Min-Sum LDPC 译码
        -> syndrome 校验
        -> SHA-256 常数时间比较
        -> 接受或拒绝
```

Normalized Min-Sum 使用：

- LLR 幅值：`4.0`；
- 归一化系数：`0.75`；
- 最大迭代次数：`30`；
- 每个测试样本映射到一个 Ascend C Core。

## 目录结构

```text
.
├── CMakeLists.txt       # CMake/Ascend C 构建配置
├── fuzzy_ldpc.asc       # Ascend C Kernel、Host 参考译码器和测试入口
├── host_crypto.cpp      # Host SHA-256 与常数时间比较
├── host_crypto.h
├── run-tests.sh         # 一键构建和测试脚本
└── README.md
```

运行后生成的 `build/`、`cceprint/`、`npuchk/` 和日志文件已由 `.gitignore` 排除。

## 修改 LDPC 参数

当前 QC-LDPC 参数位于 `fuzzy_ldpc.asc` 顶部：

```cpp
constexpr int32_t kLiftSize = 16;
constexpr int32_t kBaseRows = 3;
constexpr int32_t kBaseColumns = 6;
constexpr int32_t kMaxIterations = 30;
constexpr float kChannelLlr = 4.0F;
constexpr float kNormalization = 0.75F;
```

QC 基图循环移位由 `kBaseShifts` 定义。修改码长、基图或度数后，还需要同步检查图构造、Local Memory 大小和测试数据生成逻辑。

## 常见问题

### 找不到 ASC 或 CANN

如果 CMake 报告找不到 `ASC`，请先确认 CANN 环境已激活：

```bash
source /path/to/cann/set_env.sh
echo "$ASCEND_HOME_PATH"
```

### 找不到 OpenSSL

Ubuntu/Debian 可以安装：

```bash
sudo apt-get install libssl-dev
```

### 没有 NPU 驱动

本项目使用 `CMAKE_ASC_RUN_MODE=cpu`，CPU 孪生测试不需要 NPU 驱动。没有真实 NPU 时无法评价实际吞吐率、时延和硬件精度。

## 当前限制

- 这是正确性原型，不是生产级生物特征保护系统；
- LDPC 码型和输入长度目前固定；
- Kernel 尚未针对真实 NPU 完成向量化和 Tiling 优化；
- 当前使用 CPU 孪生环境，不能代表真实 Ascend NPU 性能；
- SHA-256 和最终认证保留在 Host，不在 AI Core 中执行；
- 实际应用必须评估模板最小熵、误码分布、辅助数据泄露、重放和侧信道风险。

## 参考项目

本项目参考了以下开源实现的算法组织方式，没有直接复制其 CUDA/PyTorch 源码：

- [robertwgh/cuLDPC](https://github.com/robertwgh/cuLDPC)（Apache-2.0）：CUDA QC-LDPC Min-Sum/SPA；
- [NVlabs/sionna-rk](https://github.com/NVlabs/sionna-rk)（Apache-2.0）：CUDA LDPC 最小值/次小值校验节点更新；
- [Lin-X-H/WCFC](https://github.com/Lin-X-H/WCFC)（MIT）：LDPC 编码、特征 XOR、译码和 SHA-256 验证流程。
