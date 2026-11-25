#pragma once 
#include <string>
#include <mosquitto.h>

class MQTTClient {

public:
MQTTClient();
~MQTTClient();

bool connect(const char* host, int port);
void poll();    
void draw();    

private:
    struct mosquitto* mosq = nullptr;
    bool connected = false;
    std::string last_topic;
    std::string last_message;

    // Callbacs
    static void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
    static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};