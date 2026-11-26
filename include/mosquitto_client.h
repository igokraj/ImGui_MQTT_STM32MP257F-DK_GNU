#pragma once 
#include <string>
#include <mosquitto.h>
#include <chrono>

class MQTTClient {

public:
MQTTClient();
~MQTTClient();

bool publish(const char *topic, const char *message, int qos = 0, bool retain = true);
bool connect(const char* host, int port);
void poll();    
void draw();    


private:
    std::string host = "localhost";
    int port = 1883;
    std::chrono::steady_clock::time_point last_attempt;
    struct mosquitto* mosq = nullptr;
    bool connected = false;
    std::string last_topic;
    std::string last_message;
    char pub_message[256] = "";
    char pub_topic[128] = "";
    std::string pub_status;

    static void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code);

    // Callbacks
    static void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
    static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};