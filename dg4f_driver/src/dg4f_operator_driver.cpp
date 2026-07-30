#include <memory>
#include <thread>


#include <iostream>
#include "dg4f_driver/dg4f_operator_TCP.hpp"
#include "rclcpp/executors/multi_threaded_executor.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

// One name per motor, in motor order, matching dg4f_ros2_control.xacro.
// This file previously carried the DG-5F left/right 20-joint lists; the DG-4F
// has 18 actuators and no handedness.
std::vector<std::string> joint_names = {
    "j_dg_1_1", "j_dg_1_2", "j_dg_1_3", "j_dg_1_4", "j_dg_2_1", "j_dg_2_2",
    "j_dg_2_3", "j_dg_2_4", "j_dg_3_1", "j_dg_3_2", "j_dg_3_3", "j_dg_3_4",
    "j_dg_4_1", "j_dg_4_2", "j_dg_4_3", "j_dg_4_4", "j_dg_1_inner",
    "j_dg_4_inner"};

class dg4fDriver : public rclcpp::Node {
 public:
  dg4fDriver() : Node("dg4f_operator_driver") {
    this->declare_parameter<std::string>("ip", "169.254.186.72");
    this->declare_parameter<int>("port", 502);
    this->declare_parameter<std::string>("hand_type", "right");

    this->get_parameter("ip", ip_);
    this->get_parameter("port", port_);
    this->get_parameter("joint_prefix", joint_prefix_);
    this->get_parameter("hand_type", hand_type_);

    publisher_ = this->create_publisher<sensor_msgs::msg::JointState>(
        "/joint_states", 10);

    subscription_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
        "/taget_joint", 10,
        std::bind(&dg4fDriver::topic_callback, this, std::placeholders::_1));

    timer_ = this->create_wall_timer(
        50ms, std::bind(&dg4fDriver::timer_callback, this));

    delto_client_ = std::make_unique<DG4F_TCP>(ip_, port_);
    if (!delto_client_->connect()) {
      RCLCPP_ERROR(this->get_logger(),
                   "Failed to connect to %s:%d", ip_.c_str(), port_);
    }
  }

  ~dg4fDriver() {
    // Cancel timers first
    if (timer_) {
      timer_->cancel();
    }

    // delto_client_->disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    delto_client_.reset();
  }

 private:
  void topic_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg) {
    // Handle the incoming message

    std::cout << "Received target joint: ";
    for (const auto& value : msg->data) {
      std::cout << value << " ";
    }

    // NOTE: msg->data is printed but never applied -- this driver has no
    // set_position_rad() call, so commanded targets are discarded.
    try {
      delto_client_->start_control();
    } catch (const std::exception& e) {
      RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
                           "Modbus write failed: %s", e.what());
    }
    std::cout << std::endl;
  }

  void timer_callback() {
    // A Modbus timeout or exception response throws. Left uncaught it
    // propagates out of the executor and terminates the process, so one
    // dropped frame used to kill the node.
    try {
      data = delto_client_->get_data();
    } catch (const std::exception& e) {
      RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
                           "Modbus read failed: %s", e.what());
      return;
    }
    // this->get_logger().info("Received data fr som DG4F");S?
    RCLCPP_INFO(this->get_logger(), "Received data from DG4F");
    // Publish joint states
    auto joint_state = sensor_msgs::msg::JointState();
    joint_state.header.stamp = this->get_clock()->now();

    joint_state.name = joint_names;
    joint_state.position = data.position;
    joint_state.velocity = data.velocity;
    joint_state.effort = data.current;
    publisher_->publish(joint_state);
  }

  std::string ip_;
  int port_;
  std::string joint_prefix_;
  std::unique_ptr<DG4F_TCP> delto_client_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr
      subscription_;
  ReceivedData data;
  std::string hand_type_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  rclcpp::executors::MultiThreadedExecutor executor(rclcpp::ExecutorOptions(),
                                                    2);

  try {
    auto node = std::make_shared<dg4fDriver>();
    executor.add_node(node);
    executor.spin();
  } catch (const std::exception& e) {
    RCLCPP_ERROR(rclcpp::get_logger("dg4fDriver"),
                 "Fatal error: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }

  rclcpp::shutdown();
  return 0;
}
