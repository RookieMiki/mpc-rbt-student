#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float32.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class MyPublisher : public rclcpp::Node
{
public:
  MyPublisher() : Node("muj_super_node")
  {
    // DEKLARACE PARAMETRŮ - zde nastavujeme defaultní hodnoty z bodu 5
    this->declare_parameter<double>("battery_max_voltage", 42.0);
    this->declare_parameter<double>("battery_min_voltage", 32.0);

    string_publisher_ = this->create_publisher<std_msgs::msg::String>("node_name", 10);
    timer_ = this->create_wall_timer(
      500ms, std::bind(&MyPublisher::timer_callback, this));

    battery_pub_ = this->create_publisher<std_msgs::msg::Float32>("battery_percentage", 10);
    battery_sub_ = this->create_subscription<std_msgs::msg::Float32>(
      "battery_voltage", 10, std::bind(&MyPublisher::battery_callback, this, _1));
  }

private:
  void timer_callback()
  {
    auto message = std_msgs::msg::String();
    message.data = this->get_name();
    string_publisher_->publish(message);
  }

  void battery_callback(const std_msgs::msg::Float32::SharedPtr msg)
  {
    // NAČTENÍ PARAMETRŮ - při každé zprávě se podíváme, jaké jsou aktuální hodnoty
    double max_v = this->get_parameter("battery_max_voltage").as_double();
    double min_v = this->get_parameter("battery_min_voltage").as_double();

    float voltage = msg->data;
    
    // VÝPOČET - teď už používáme proměnné místo fixních čísel
    float percentage = 0.0;
    if (max_v > min_v) {
      percentage = (voltage - min_v) / (max_v - min_v) * 100.0;
    }

    // Bezpečnostní oříznutí
    if (percentage > 100.0) percentage = 100.0;
    if (percentage < 0.0) percentage = 0.0;

    auto out_msg = std_msgs::msg::Float32();
    out_msg.data = percentage;
    
    // Vylepšený výpis, ať vidíme, že to počítá se správnými parametry
    RCLCPP_INFO(this->get_logger(), "Napeti: %.2f V -> %.1f %% (Pocitano z rozsahu %.1f - %.1f V)", 
                voltage, percentage, min_v, max_v);
    
    battery_pub_->publish(out_msg);
  }
  
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr string_publisher_;
  rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr battery_pub_;
  rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr battery_sub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyPublisher>());
  rclcpp::shutdown();
  return 0;
}
