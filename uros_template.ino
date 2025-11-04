#include <micro_ros_arduino.h>
// #include <micro_ros_transport_wifi.h>
#include <WiFi.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

char ssid[] = "SHARKY_2.4GHz";
char password[] = "fliperama2309";

// IP do computador onde roda o micro-ros-agent
// char agent_ip[] = "192.168.0.104";  // Lima-2G @ UFMG
char agent_ip[] = "192.168.100.8";  // SHARKY_2.4GHz @ Home
const int agent_port = 8888;

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_executor_t executor;
rcl_timer_t timer;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
 RCLC_UNUSED(last_call_time);
 if (timer != NULL) {
   msg.data++;
   rcl_publish(&publisher, &msg, NULL);
   Serial.print("Mensagem publicada: ");
   Serial.println(msg.data);
 }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP do ESP32: ");
  Serial.println(WiFi.localIP());

  set_microros_wifi_transports(ssid, password, agent_ip, agent_port);

 allocator = rcl_get_default_allocator();

 // Inicialização do micro-ROS
 rclc_support_init(&support, 0, NULL, &allocator);
 rclc_node_init_default(&node, "micro_ros_esp32_node", "", &support);
 rclc_publisher_init_default(
     &publisher,
     &node,
     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
     "topic_esp32");

 rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(1000), timer_callback);

 rclc_executor_init(&executor, &support.context, 1, &allocator);
 rclc_executor_add_timer(&executor, &timer);

 msg.data = 0;
}

void loop() {
 rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  delay(100);
}
