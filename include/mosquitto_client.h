#pragma once 
#include <string>
#include <mosquitto.h>
#include <chrono>
#include <deque>

struct Message {
    std::string time;
    std::string topic;
    std::string payload;
};



class MQTTClient {

public:
MQTTClient();
~MQTTClient();

bool publish(const char *topic, const char *message, int qos = 0, bool retain = false);
bool connect(const char* host, int port);
void poll();    
void draw();
void draw_history();

private:
    std::string host = "localhost";
    int port = 1883;
    std::chrono::steady_clock::time_point last_attempt;
    struct mosquitto* mosq = nullptr;
    bool connected = false;
    std::string last_topic;
    std::string last_message;
    std::deque<Message> history;
    char pub_message[256] = "";
    char pub_topic[128] = "";
    char sub_topic[128] = "home/temperature";
    std::string pub_status;
    std::string sub_status;
    char sub_input[128];
    bool retain = false;
    int qos_pub = 0;
    int qos_sub = 0;
    


    // Callbacks
    static void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code);
    static void on_connect(struct mosquitto *mosq,  void *obj, int reason_code);
    static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};