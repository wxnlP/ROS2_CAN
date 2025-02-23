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
            false
        );
        timer_ = this->create_wall_timer(500ms, std::bind(&CanReceiver::TimerCallback, this));
    }


private:
    void TimerCallback(void)
    {
        try {
            RCLCPP_INFO(this->get_logger(), "回调函数！");
            unsigned char message[8] = {0};
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