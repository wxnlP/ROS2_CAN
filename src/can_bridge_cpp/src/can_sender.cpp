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
    CanSender(std::string name) : Node(name)
    {
        RCLCPP_INFO(this->get_logger(), "节点的名称为:%s.", name.c_str());
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
                StandardFrame_{}));
        /* 创建定时器 */
        timer_ = this->create_wall_timer(1s, std::bind(&CanSender::TimerCallback, this));
    }

private:
    /* 定时器回调函数 */
    void TimerCallback(void)
    {
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
            100ms);
    }
    std::shared_ptr<drivers::socketcan::SocketCanSender> can_sender_;
    rclcpp::TimerBase::SharedPtr timer_;
    
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CanSender>("can_sender");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}