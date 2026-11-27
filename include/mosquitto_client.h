#pragma once 
#include <string>
#include <mosquitto.h>
#include <chrono>
#include <deque>
#include <vector>

struct Message_received {
    std::string time;
    std::string topic;
    std::string payload;
    int qos;
};

struct Message_published {
    std::string time;
    std::string topic;
    std::string payload;
    bool retain;
    int qos;
};

class MQTTClient {

public:
MQTTClient();
~MQTTClient();

bool publish(const char *topic, const char *message, int qos = 0, bool retain = false);
bool connect(const char* host, int port);
void poll();    
void draw();
void draw_history_received();
void draw_history_published();
void draw_subscriptions();
void draw_plot();

private:

    // Broker ID
    std::string host = "localhost";
    int port = 1883;

    // Messages
    std::chrono::steady_clock::time_point last_attempt;
    struct mosquitto* mosq = nullptr;
    bool connected = false;
    std::string last_topic;
    std::string last_message;

    // Logs
    std::deque<Message_received> history_rec;
    std::deque<Message_published> history_pub;

    // Plot
    std::vector<double> temp_values;
    std::vector<double> temp_times;

    // Publish
    char pub_message[256] = "";
    char pub_topic[128] = "";
    std::string pub_status;
    int qos_pub = 0;

    // Subscribe
    std::vector<std::string> sub_topics;
    std::string sub_status;
    char sub_input[128];
    bool retain = false;
    int qos_sub = 0;

    // Callbacks
    static void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code);
    static void on_connect(struct mosquitto *mosq,  void *obj, int reason_code);
    static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};