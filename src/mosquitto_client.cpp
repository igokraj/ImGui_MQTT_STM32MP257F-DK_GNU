#include "mosquitto.h"
#include "imgui.h"
#include "mosquitto_client.h"
#include <string>
#include <iostream>
#include <cstring>
#include <ctime>

MQTTClient::MQTTClient()
{
    mosquitto_lib_init();
    // Pass 'this' (address of this MQTTClient object) as user data.
    // libmosquitto stores it and gives it back as 'obj' in every callback.
    mosq = mosquitto_new(nullptr, true, this);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
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

bool MQTTClient::connect(const char *h, int p)
{
    host = h;
    port = p;
    last_attempt = std::chrono::steady_clock::now();
    return mosquitto_connect(mosq, host.c_str(), port, 60) == MOSQ_ERR_SUCCESS;
}

void MQTTClient::poll()
{
    if (!connected)
    {
        auto now = std::chrono::steady_clock::now();
        if (now - last_attempt > std::chrono::seconds(2))
        {
            last_attempt = now;
            mosquitto_connect(mosq, host.c_str(), port, 60);
        }
    }
    mosquitto_loop(mosq, 0, 1);
}

/* Callback called when the client receives a CONNACK message from the broker. */
void MQTTClient::on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    MQTTClient *self = static_cast<MQTTClient *>(obj);
    self->connected = (reason_code == 0);
    if (reason_code == 0)
    {
        mosquitto_subscribe(mosq, nullptr, self->sub_topic, self->qos_sub);
    }
}

/* Callback called when the broker is disconnected. */
void MQTTClient::on_disconnect(struct mosquitto *mosq, void *obj, int reason_code)
{
    MQTTClient *self = static_cast<MQTTClient *>(obj);
    self->connected = false;
}

/* Callback called when the client receives a message. */
void MQTTClient::on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    MQTTClient *self = static_cast<MQTTClient *>(obj);
    self->last_topic = msg->topic;
    self->last_message.assign((const char *)msg->payload, msg->payloadlen);

    // Make a message for the logs
    std::time_t now = std::time(nullptr);
    std::tm *t = std::localtime(&now);
    char time[16];
    std::strftime(time, sizeof(time), "%H:%M:%S", t);

    Message m;
    m.time = time;
    m.topic = msg->topic;
    m.payload = std::string((const char *)msg->payload, msg->payloadlen);
    self->history.push_back(m);

    if (self->history.size() > 50)
    {
        self->history.pop_front();
    }
}

void MQTTClient::draw()
{
    ImGui::SeparatorText("Status");

    if (connected)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected");
        ImGui::Text("Connected to %s:%d", host.c_str(), port);
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Disconnected");
        ImGui::Text("Trying to connect to %s:%d...", host.c_str(), port);
    }

    // ********************************************************

    ImGui::Spacing();
    ImGui::SeparatorText("Subscription");

    ImGui::InputText("Topic##sub", sub_input, sizeof(sub_input));
    ImGui::Text("Subscribed topic: %s", sub_topic);

    // Change qos imidiately after user sets it
    bool qos_changed = false;
    if (ImGui::RadioButton("QoS 0##sub", &qos_sub, 0))
    {
        qos_changed = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("QoS 1##sub", &qos_sub, 1))
    {
        qos_changed = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("QoS 2##sub", &qos_sub, 2))
    {
        qos_changed = true;
    }

    if (qos_changed && connected && sub_topic[0] != '\0')
    {
        mosquitto_subscribe(mosq, nullptr, sub_topic, qos_sub);
    }

    if (ImGui::Button("Subscribe"))
    {
        if (sub_input[0] == '\0')
            sub_status = "Subscription topic is empty";
        else
        {
            mosquitto_unsubscribe(mosq, nullptr, sub_topic);
            snprintf(sub_topic, sizeof(sub_topic), "%s", sub_input);
            if (mosquitto_subscribe(mosq, nullptr, sub_topic, qos_sub) == MOSQ_ERR_SUCCESS)
                sub_status = "Topic subscribed!";
            else
                sub_status = "Subscribe failed";
        }
    }
    if (!sub_status.empty())
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(sub_status.c_str());
    }

    // ********************************************************

    ImGui::Spacing();
    ImGui::SeparatorText("Received messages");

    ImGui::Text("Topic: %s", last_topic.c_str());
    ImGui::Text("Message: %s", last_message.c_str());

    // ********************************************************

    ImGui::Spacing();
    ImGui::SeparatorText("Publish");

    ImGui::InputText("Topic##pub", pub_topic, sizeof(pub_topic));
    ImGui::InputText("Message##pub", pub_message, sizeof(pub_message));

    int length_left = (int)(sizeof(pub_message) - 1 - strlen(pub_message));
    if (length_left > 0)
        ImGui::Text("Characters left: %d", length_left);
    else
        ImGui::Text("Message is too long!");

    ImGui::RadioButton("QoS 0##pub", &qos_pub, 0);
    ImGui::SameLine();
    ImGui::RadioButton("QoS 1##pub", &qos_pub, 1);
    ImGui::SameLine();
    ImGui::RadioButton("QoS 2##pub", &qos_pub, 2);

    // Choose to retain the message on the topic or not
    ImGui::SameLine();
    ImGui::Checkbox("Retain", &retain);

    if (ImGui::Button("Send"))
    {
        if (pub_message[0] == '\0' && pub_topic[0] == '\0')
            pub_status = "Message and Topic are empty";
        else if (pub_message[0] == '\0')
            pub_status = "Message is empty";
        else if (pub_topic[0] == '\0')
            pub_status = "Topic is empty";
        else if (publish(pub_topic, pub_message, qos_pub, retain))
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

void MQTTClient::draw_history()
{

    if (ImGui::Button("Clear"))
    {
        history.clear();
    }
    ImGui::BeginChild("History", ImVec2(0, 0), ImGuiChildFlags_Borders);

    for (const auto &m : history)
    {
        ImGui::Text("[%s] %s : %s", m.time.c_str(), m.topic.c_str(), m.payload.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}
