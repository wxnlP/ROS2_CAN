# ROS2_CAN
## 1. can_utils工具

`can-utils` 是一套用于Linux操作系统的开源工具，专门用来处理与CAN（Controller Area Network）总线相关的任务。CAN总线广泛应用于汽车和工业自动化中，用于设备之间的通讯。 这个工具集提供了多种命令行工具，用于发送、接收和处理CAN网络上的数据。

`can_utils` 的基本使用：

- **candump**
  - **功能** ：捕获并显示通过CAN接口的数据帧。
  - `candump can0` 显示 `can0` 接口上的所有CAN数据。
  - `candump can0,123:7FF` 仅显示ID范围在 `0x123` 到 `0x7FF` 之间的CAN数据。
  - `candump -l can0`记录数据到文件，默认文件名格式为 `candump-日期.log`。
- **cansend**
  - **功能** ：向CAN总线发送CAN消息。
  - `cansend can0 000#1122334455667788` 向 `can0` 接口发送一个ID为 `0x000` 的CAN帧，数据为 `1122334455667788`。
- **cangen**
  - **功能** ：生成随机CAN数据包并发送到CAN总线上。
  - `cangen can0` 生成并发送随机CAN数据到 `can0` 接口。
- **canplayer**
  - **功能** ：重放 `candump` 记录的数据。
  - `canplayer -I log.txt` 重放存储在 `log.txt` 文件中的CAN数据。
- **cansniffer**
  - **功能** ：显示CAN数据的变化，类似于Wireshark。
  - `cansniffer can0` 显示 `can0` 接口上的CAN数据变化。

配置启动CAN接口：

```shell
# 关闭CAN0接口
sudo ip link set can0 down
# 配置CAN接口波特率 500000
sudo ip link set can0 type can bitrate 500000
# 配置回环测试(自发自收)模式
sudo ip link set can0 type can loopback on
# 启动CAN0接口
sudo ip link set can0 up
```

CAN通信原理推荐阅读：[第十二章 CAN - 小李 的知识库](https://tonmoon.top/study/STM32/12.CAN通信/)

## 2. ros2_socketcan收发示例

### 2.1 安装ROS2功能包

```shell
sudo apt install ros-humble-can-msgs
sudo apt install ros-humble-ros2-socketcan
```

进入工作空间src目录，创建CAN通信的功能包

```shell
ros2 pkg create can_bridge_cpp --build-type ament_cmake --dependencies rclcpp ros2_socketcan ros2_socketcan_msgs --license Apache-2.0
```



### 2.2 CAN发送

```C++
#include "rclcpp/rclcpp.hpp"
#include "ros2_socketcan/socket_can_sender.hpp"
#include "can_msgs/msg/frame.hpp"
// can发送者工作空间
using namespace drivers::socketcan;
// 定时器时间参数工作空间
using namespace std::chrono_literals;

class CanSender : public rclcpp::Node
{
public:
    /* 构造函数 */
    CanSender(std::string name) : Node(name) {
        RCLCPP_INFO(this->get_logger(), "节点的名称为:%s.",name.c_str());
        /* 创建发送者 */
        can_sender_ = std::make_shared<SocketCanSender>(
            // can接口
            "can0",
            // 启用can_fd
            false,
            // 默认配置
            CanId(
                // 默认ID
                0x123, 
                // 时间戳，0表示立即发送
                0,
                // 数据帧、遥控帧、错误帧
                FrameType::DATA,
                // 标准格式、扩展格式
                StandardFrame_{}) 
        );
        /* 创建定时器 */
        timer_ = this->create_wall_timer(1s, std::bind(&CanSender::TimerCallback, this));
    }

private:
    /* 定时器回调函数 */
    void TimerCallback(void) {
        RCLCPP_INFO(this->get_logger(), "回调函数！");
        /* 初始化发送的消息 */
        unsigned char messages[] = {0xCE, 0xBD, 0xEE, 0xFF};
        can_sender_->send(
            // 必要参数
            messages,
            // 灵活配置(选配)
            CanId(
                // 默认ID
                0x126, 
                // 时间戳，0表示立即发送
                0,
                // 数据帧、遥控帧、错误帧
                FrameType::DATA,
                // 标准格式、扩展格式
                StandardFrame_{}),
            // 选配
            100ms
        );
    }
    std::shared_ptr<drivers::socketcan::SocketCanSender> can_sender_;
    std::shared_ptr<drivers::socketcan::SocketCanReceiver> can_receiver_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CanSender>("can_sender");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

配置`CMakeLists.txt`文件，如下是需添加内容：

```cmake
add_executable(can_sender_cpp src/can_sender.cpp)

ament_target_dependencies(
  can_sender_cpp
  "rclcpp"
  "ros2_socketcan"
  "ros2_socketcan_msgs"
)

install(TARGETS
can_sender_cpp
  DESTINATION lib/${PROJECT_NAME}
)
```

运行结果：

![image-20250223140554986](README/image-20250223140554986.png)

### 2.3 CAN接收

```C++
#include "rclcpp/rclcpp.hpp"
#include "ros2_socketcan/socket_can_receiver.hpp"
#include "can_msgs/msg/frame.hpp"

// can接收者者工作空间
using namespace drivers::socketcan;
// 定时器时间参数工作空间
using namespace std::chrono_literals;

class CanReceiver : public rclcpp::Node
{
public:
    /* 构造函数 */
    CanReceiver(std::string name) : Node(name)
    {
        RCLCPP_INFO(this->get_logger(), "节点的名称为:%s.", name.c_str());
        can_receiver_ = std::make_shared<SocketCanReceiver>(
            "can0",
            // 是否使用高速CAN
            false
        );
        timer_ = this->create_wall_timer(500ms, std::bind(&CanReceiver::TimerCallback, this));
    }


private:
    void TimerCallback(void)
    {
        try {
            RCLCPP_INFO(this->get_logger(), "回调函数！");
            unsigned char message[4] = {0};
            can_id = can_receiver_->receive(message, 100us);
            RCLCPP_INFO(
                this->get_logger(),
                "ID：%d, 是否扩展格式：%s, 帧格式：%d, 数据长度：%u",
                can_id.get(), 
                can_id.is_extended() ? "是" : "否", 
                static_cast<int>(can_id.frame_type()),
                can_id.length()
            );
            uint32_t length = can_id.length();
            for (uint32_t i = 0 ; i < length ; i++) {
                RCLCPP_INFO(
                this->get_logger(), 
                "数据[%d]：%d",
                i,
                message[i]
                );
            }
        }
        catch (const SocketCanTimeout& e) {
            RCLCPP_DEBUG(this->get_logger(), "接收超时（正常现象）");
        }
        catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "接收错误: %s", e.what());
        }
    }
    std::shared_ptr<drivers::socketcan::SocketCanReceiver> can_receiver_;
    rclcpp::TimerBase::SharedPtr timer_;
    CanId can_id;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CanReceiver>("can_receiver");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

⚠有一个问题就是，`unsigned char message[4] = {0};`接收数据的数组要和发送的字节数一致，否则会报一个错：

![image-20250223170924849](README/image-20250223170924849.png)

当然这样做个人认为并非好的处理方式，但暂时还未找到合适的解决方案。只能发送方按传统CAN协议的最大字节数 8 ，没有用的位置 `0x00`。

运行结果，`291`为报文ID：

![image-20250223171250116](README/image-20250223171250116.png)