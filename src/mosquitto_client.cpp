#include "mosquitto.h"
#include "imgui.h"
#include "mosquitto_client.h"
#include <string>
#include <iostream>
#include <cstring>

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

bool MQTTClient::publish(const char *topic, const char *message, int qos, bool retain)
{
    if (!connected)
        return false;

    int rc = mosquitto_publish(mosq, nullptr, topic,
                               (int)strlen(message), message,
                               qos, retain);
    return rc == MOSQ_ERR_SUCCESS;
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
    if (connected)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected");
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Disconnected");

    ImGui::Text("Topic: %s", last_topic.c_str());
    ImGui::Text("Message: %s", last_message.c_str());

    ImGui::Separator();

    ImGui::InputText("Topic##pub", pub_topic, sizeof(pub_topic));
    ImGui::InputText("Message##pub", pub_message, sizeof(pub_message));

    int length_left = (int)(sizeof(pub_message) - 1 - strlen(pub_message));
    ImGui::Text("Characters left: %d", length_left);

    if (ImGui::Button("Send"))
    {
        if (pub_message[0] == '\0' && pub_topic[0] == '\0')
            pub_status = "Message and Topic are empty";
        else if (pub_message[0] == '\0')
            pub_status = "Message is empty";
        else if (pub_topic[0] == '\0')
            pub_status = "Topic is empty";
        else if (publish(pub_topic, pub_message))
            pub_status = "Sent";
        else
            pub_status = "Send failed";
    }

    if (!pub_status.empty())
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(pub_status.c_str());
    }
}
