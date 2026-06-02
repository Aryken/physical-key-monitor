# 实体按键监控系统 (Physical Key Monitor) - C Version

Linux 平台的实时物理按键监控解决方案，采用 C 语言开发，支持键盘、按钮、开关的状态采集、异常检测、可视化展示。

## 🎯 核心功能

- **实时采集**: 通过 evdev 驱动监控所有物理按键的按下/释放状态
- **数据记录**: SQLite 数据库存储完整的按键事件日志和时间序列数据
- **异常检测**: 识别按键故障、异常触发、频率异常等问题
- **可视化展示**: Web 仪表板实时显示按键状态和统计数据
- **告警系统**: 异常情况实时报警和通知机制

## 📦 系统架构

```
Physical Key Monitor (C)
├── 采集层 (src/core/)
│   ├── input_device.c      # Linux evdev 设备管理
│   ├── event_listener.c    # 事件监听和采集
│   └── event_queue.c       # 线程安全的事件队列
├── 处理层 (src/processor/)
│   ├── event_processor.c   # 事件解析处理
│   ├── anomaly_detector.c  # 异常检测算法
│   └── state_tracker.c     # 按键状态跟踪
├── 存储层 (src/storage/)
│   ├── database.c          # SQLite 数据库操作
│   ├── event_store.c       # 事件存储管理
│   └── db_schema.sql       # 数据库架构
├── 分析层 (src/analysis/)
│   ├── statistics.c        # 数据统计分析
│   └── reporter.c          # 报告生成
├── 网络/API (src/api/)
│   ├── http_server.c       # 简单 HTTP 服务器
│   ├── rest_api.c          # REST API 实现
│   └── websocket.c         # WebSocket 推送
└── 主程序 (src/)
    ├── main.c              # 主程序入口
    ├── monitor.c           # 监控核心
    └── config.c            # 配置管理
```

## 🚀 快速开始

### 系统要求

- Linux (Ubuntu 18.04+ / CentOS 7+)
- GCC 7.0+
- SQLite3 开发库
- libevdev 库
- libevent (for HTTP server)
- pthread (standard)

### 依赖安装

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential cmake
sudo apt-get install -y libsqlite3-dev libevdev-dev libevent-dev

# CentOS/RHEL
sudo yum groupinstall -y "Development Tools"
sudo yum install -y sqlite-devel libevdev-devel libevent-devel
```

### 编译

```bash
cd physical-key-monitor
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### 运行

```bash
# 开发环境
./build/bin/physical-key-monitor --config ../config.yaml --debug

# 生产环境（需要 root 权限以访问输入设备）
sudo ./build/bin/physical-key-monitor --config config.yaml --daemon
```

### 访问仪表板

Web 界面: http://localhost:8080

## 📋 配置文件

编辑 `config.yaml`:

```yaml
monitor:
  devices:
    - /dev/input/event0
    - /dev/input/event1
  
  sampling_interval_ms: 100
  history_retention_days: 7
  
anomaly_detection:
  enabled: true
  frequency_threshold: 50          # 次/秒
  stuck_detection_time_sec: 30
  burst_threshold: 100
  response_time_threshold_ms: 5000

database:
  type: sqlite
  path: /var/lib/physical-key-monitor/events.db
  max_connections: 10

http_server:
  host: 0.0.0.0
  port: 8080
  worker_threads: 4

logging:
  level: INFO                       # DEBUG | INFO | WARN | ERROR
  file: /var/log/physical-key-monitor/monitor.log
  max_size_mb: 100
  backup_count: 5
```

## 📊 数据模型

### 事件结构
```c
struct key_event {
    uint64_t timestamp;          // 毫秒时间戳
    char device_path[256];       // 设备路径
    uint16_t key_code;           // 按键代码
    char key_name[64];           // 按键名称
    uint8_t state;               // 0: 释放, 1: 按下, 2: 重复
    uint32_t duration_ms;        // 按键持续时间
};
```

### 告警结构
```c
struct alert {
    uint32_t alert_id;
    uint32_t type;               // 异常类型
    uint8_t severity;            // 严重级别: 0-3
    char message[512];
    uint64_t timestamp;
    uint16_t key_code;
    char key_name[64];
};
```

## 🔧 API 端点

```bash
# 获取监控状态
curl http://localhost:8080/api/v1/status

# 获取设备列表
curl http://localhost:8080/api/v1/devices

# 获取按键状态
curl http://localhost:8080/api/v1/keys

# 获取最近事件
curl http://localhost:8080/api/v1/events?limit=100

# 获取告警
curl http://localhost:8080/api/v1/alerts?severity=high

# 导出数据
curl http://localhost:8080/api/v1/export?format=csv&duration=24h > report.csv
```

## 🐛 故障排查

### 无法访问输入设备

```bash
# 检查权限
ls -la /dev/input/

# 方案 1: 添加 udev 规则（推荐）
sudo tee /etc/udev/rules.d/99-physical-key-monitor.rules > /dev/null << EOF
KERNEL=="event*", SUBSYSTEM=="input", MODE="0666"
EOF

sudo udevadm control --reload-rules
sudo udevadm trigger

# 方案 2: 临时运行
sudo ./build/bin/physical-key-monitor
```

### 查看日志

```bash
# 实时日志
tail -f /var/log/physical-key-monitor/monitor.log

# 搜索错误
grep ERROR /var/log/physical-key-monitor/monitor.log
```

## 📦 部署

### Systemd 服务

```bash
sudo cp config/physical-key-monitor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable physical-key-monitor
sudo systemctl start physical-key-monitor
sudo systemctl status physical-key-monitor
```

### 查看服务状态

```bash
sudo systemctl status physical-key-monitor
sudo journalctl -u physical-key-monitor -f
```

## 📈 性能指标

- 事件处理延迟: < 1ms
- 内存占用: ~50-100MB
- CPU 使用率: < 5% (idle)
- 支持设备数: 32+
- 数据库查询速度: < 100ms

## 🔐 安全性

- 最小权限原则（仅需访问 /dev/input）
- 事件队列大小限制（防止 DoS）
- 数据库连接池管理
- 日志脱敏（隐藏敏感按键信息）

## 📝 编译选项

```bash
# 调试版本
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 发布版本
cmake -DCMAKE_BUILD_TYPE=Release ..

# 启用所有调试符号
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..

# 启用 AddressSanitizer（检测内存问题）
cmake -DENABLE_ASAN=ON ..
```

## 👥 贡献

欢迎提交 Issue 和 Pull Request！

---

**最后更新**: 2026-06-02
