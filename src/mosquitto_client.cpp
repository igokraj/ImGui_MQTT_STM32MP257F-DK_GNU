#include "mosquitto.h"
#include "imgui.h"
#include "mosquitto_client.h"
#include <string>

#define subscribe_topic "home/temperature"

MQTTClient::MQTTClient()
{
    mosquitto_lib_init();
    mosq = mosquitto_new(nullptr, true, this);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
}

MQTTClient::~MQTTClient()
{
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

bool MQTTClient::connect(const char *host, int port)
{
    return mosquitto_connect(mosq, host, port, 60) == MOSQ_ERR_SUCCESS;
}

void MQTTClient::poll()
{
    if (mosquitto_loop(mosq, 0, 1) != MOSQ_ERR_SUCCESS)
    {
        connected = false;
    }
}

/* Callback called when the client receives a CONNACK message from the broker. */
void MQTTClient::on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    MQTTClient *self = static_cast<MQTTClient *>(obj);
    self->connected = (reason_code == 0);
    if (reason_code == 0)
    {
        mosquitto_subscribe(mosq, nullptr, subscribe_topic, 0);
    }
}

/* Callback called when the client receives a message. */
void MQTTClient::on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    MQTTClient *self = static_cast<MQTTClient *>(obj);
    self->last_topic = msg->topic;
    self->last_message.assign((const char *)msg->payload, msg->payloadlen);
}

void MQTTClient::draw()
{

    // Set the position of the window at the start of the app
    ImGui::SetNextWindowPos(ImVec2(1000, 500), ImGuiCond_Once);
    ImGui::Begin("MQTT", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    if (connected) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected");
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Disconnected");
    }

    ImGui::Text("Topic: %s", (last_topic).c_str());
    ImGui::Text("Message: %s", (last_message).c_str());

    ImGui::End();    
}
