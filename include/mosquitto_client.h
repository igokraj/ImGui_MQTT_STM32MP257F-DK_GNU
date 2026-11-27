#pragma once
#include <string>
#include <mosquitto.h>
#include <chrono>
#include <deque>
#include <vector>

struct Message_received
{
    std::string time;
    std::string topic;
    std::string payload;
    int qos;
};

struct Message_published
{
    std::string time;
    std::string topic;
    std::string payload;
    bool retain;
    int qos;
};

struct PlotData
{
    std::vector<double> values;
    std::vector<double> times;
};

class MQTTClient
{

public:
    MQTTClient();
    ~MQTTClient();

    bool publish(const char *topic, const char *message, int qos = 0, bool retain = false);
    bool connect(const char *host, int port);
    void poll();
    void draw();
    void draw_history_received();
    void draw_history_published();
    void draw_subscriptions();
    void draw_deepsleep_intevral();
    void draw_plot(const char *topic, const char *unit, PlotData &data);
    void draw_temp() 
    { 
        draw_plot("home/temperature", "C", temp_data); 
    }
    void draw_press() 
    { 
        draw_plot("home/preassure", "hPa", press_data); 
    }
    void draw_hum()
    {
        draw_plot("home/humidity", "%", hum_data);
    }
    void add_val(PlotData &data, const std::string &payload);

private:
    // Broker ID
    std::string host = "localhost";
    int port = 1883;

    // Messages
    std::chrono::steady_clock::time_point last_attempt;
    struct mosquitto *mosq = nullptr;
    bool connected = false;
    std::string last_topic;
    std::string last_message;

    // Logs
    std::deque<Message_received> history_rec;
    std::deque<Message_published> history_pub;

    // Plot temp
    PlotData temp_data;

    // Plot preass
    PlotData press_data;

    // Plot humidity
    PlotData hum_data;

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

    // Deep Sleep Interval
    int sleep_interval = 60;
    std::string time_status;
    bool interval_retain = false;
    int qos_interval = 0;

    // Callbacks
    static void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code);
    static void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
    static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};