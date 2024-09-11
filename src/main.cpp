#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "esp_camera.h"
#include "Freenove_4WD_Car_WiFi.h"
#include "Freenove_4WD_Car_Emotion.h"
#include "Freenove_4WD_Car_WS2812.h"
#include "Freenove_4WD_Car_For_ESP32.h"
#include <PubSubClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <limits.h>
#include <SPIFFS.h>


// Intégration de la page websocket servant a controler la voiture
#include "websocket.h"


#define CAMERA_MODEL_WROVER_KIT
#define STREAM_CONTENT_BOUNDARY "123456789000000000000987654321"

// Ressources
// https://randomnerdtutorials.com/esp32-websocket-server-arduino/#1
// https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/-
// https://randomnerdtutorials.com/esp32-static-fixed-ip-address-arduino-ide/
// https://github.com/Freenove/Freenove_4WD_Car_Kit_for_ESP32/tree/master

char *ssid_wifi = "iPhonejason";  // Le nom du réseau WiFi
char *password_wifi = "jl933602"; // Le password du WiFi

const char *mqtt_server = "172.20.10.2";
const int mqtt_port = 1883;
const int mqtt_interval_ms = 1000; // L'interval en ms entre deux envois de données
char *mqttId = "";
char *mqttUser = "";
char *mqttPass = "";


AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // Changez le nom de ce point d'accès pour "sécuriser" l'accès à votre voiture

IPAddress localIP(172, 20, 10, 3); // l'IP que vous voulez donner à votre voiture

IPAddress localGateway(172, 20, 10, 1); // L'IP de la gateway de votre réseau
IPAddress localSubnet(255, 255, 255, 240);  // Le masque de sous réseau

IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);



WiFiClient espClient;
PubSubClient client(espClient);

// WiFiServer server_Cmd(4000);
WiFiServer server_Camera(7000);
bool videoFlag = 0;

long last_message = 0;

int distance[4];          // Storage of ultrasonic data
int sensor_v;             // Int cast of track sensor data
char buff[6];             // Buffer to store the battery voltage data
char ultrasonic_buff[10]; // Buffer to store the Ultrasonic data

// put function declarations here:
void WiFi_Init();
void loopTask_Camera(void *pvParameters);
void notifyClients();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void initWebSocket();
void reconnect();
void startCameraServer();




void WiFi_Init()
{
    ssid_Router = ssid_wifi;         // Modify according to your router name
    password_Router = password_wifi; // Modify according to your router password
    ssid_AP = "Sunshine";            // ESP32 turns on an AP and calls it Sunshine
    password_AP = "Sunshine";        // Set your AP password for ESP32 to Sunshine
    frame_size = FRAMESIZE_CIF;      // 400*296
}




void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    // Serial.println((char *)data);



    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;


    }
    


    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;

        StaticJsonDocument<200> doc;

        DeserializationError error = deserializeJson(doc, (char *)data);

        if (error)
        {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        int cmd = doc["cmd"];

        if (1 == cmd)
        {
            JsonArray data = doc["data"];
            int data_0 = data[0];
            int data_1 = data[1];
            int data_2 = data[2];
            int data_3 = data[3];

            Motor_Move(data_0, data_1, data_2, data_3);
        }
        else if (2 == cmd)
        {
            int data = doc["data"];
            Emotion_SetMode(data);
        }
        else if (3 == cmd)
        {
            JsonArray angles = doc["data"];
            int angle_0 = angles[0];
            int angle_1 = angles[1];
            Servo_1_Angle(angle_0); // Set the Angle value of servo 1 to 0 to 180°
            Servo_2_Angle(angle_1);
        }
        else if (4 == cmd)
        {
            int led_mode = doc["data"];
            WS2812_SetMode(led_mode);
        }
        else if (5 == cmd)
        {
            JsonArray led_color = doc["data"];
            int led_color_0 = led_color[0];
            int led_color_1 = led_color[1];
            int led_color_2 = led_color[2];
            int led_color_3 = led_color[3];

            WS2812_Set_Color_1(led_color_0, led_color_1, led_color_2, led_color_3);
        }
        else if (6 == cmd)
        {
            JsonArray led_color_2 = doc["data"];
            int led_color_2_0 = led_color_2[0];
            int led_color_2_1 = led_color_2[1];
            int led_color_2_2 = led_color_2[2];
            int led_color_2_3 = led_color_2[3];

            WS2812_Set_Color_2(led_color_2_0, led_color_2_1, led_color_2_2, led_color_2_3);
        }
        else if (7 == cmd)
        {
            bool alarm = doc["data"] == 1;
            Buzzer_Alarm(alarm);
        }
        else if (8 == cmd)
        {
            JsonArray buzzer_data = doc["data"];
            int alarm_on = buzzer_data[0] == 1;
            int frequency_hz = buzzer_data[1];
            Buzzer_Variable(alarm_on, frequency_hz);
        }
        else if (9 == cmd)
        {
            bool video_activation = doc["data"] == 1;
            videoFlag = video_activation;
        }


        // On ajoute la commande cmd 10 qui consiste a activer le mode de suivi de ligne
        else if (10 == cmd)
        {
          int car_select = doc["data"];
            if(car_select == 1) {
            Car_Select(2);
            Serial.print("Mode suivi de ligne activé");
          }
          else if(car_select == 0) {
            Car_Select(0);
          }
            Serial.print("Mode manuel activé");
          }
        }
        


        // notifyClients();
    }


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
    
}

void initWebSocket()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}




void setup()
{
    // delay(5000);

    Serial.begin(115200);
    Serial.setDebugOutput(true);

  // Initialisation de SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Initialisation failed!");
    return;
  }
  Serial.println("SPIFFS Initialisation done.");


    if (!WiFi.config(localIP, localGateway, localSubnet, primaryDNS, secondaryDNS))
    {
        Serial.println("STA Failed to configure");
    }



    Buzzer_Setup(); // Buzzer initialization
    WiFi_Init();    // WiFi paramters initialization
    WiFi_Setup(0);  // Start AP Mode. If you want to connect to a router, change 1 to 0.
    // server_Cmd.begin(4000);    // Start the command server
    server_Camera.begin(7000); // Turn on the camera server

   // cameraSetup(); // Camera initialization
    //camera_vflip(true);
    //camera_hmirror(true);
    
    Emotion_Setup();    // Emotion initialization
    WS2812_Setup();     // WS2812 initialization
    PCA9685_Setup();    // PCA9685 initialization
    Light_Setup();      // Light initialization
    Track_Setup();      // Track initialization
    Ultrasonic_Setup(); // Initialize the ultrasonic module

    client.setServer(mqtt_server, mqtt_port);


    initWebSocket();


  server.on("/track", HTTP_GET, [](AsyncWebServerRequest *request){
    float distance = Get_Sonar();
    String json = "{\"distance\": " + String(distance) + "}";
    request->send(200, "application/json", json);
  });


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", styles_css);
  });


  server.on("/script_joystick_roues.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script_joystick_roues.js", "text/javascript");
  });
  server.on("/script_joystick_camera.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script_joystick_camera.js", "text/javascript");
  });
  server.on("/script_main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script_main.js", "text/javascript");
  });
  server.on("/script_moteur.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script_moteur.js", "text/javascript");
  });
  server.on("/script_websocket.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script_websocket.js", "text/javascript");
  });

  server.on("/script_chronometre.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script_chronometre.js", "text/javascript");
  });

    server.begin();

    // Init the state of the car
    Emotion_SetMode(1);
    WS2812_SetMode(1);
    

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");


}



void loop()
{


    // put your main code here, to run repeatedly:
    ws.cleanupClients();


    Emotion_Show(emotion_task_mode); // Led matrix display function
    WS2812_Show(ws2812_task_mode);   // Car color lights display function

    // The MQTT part
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    long now = millis();
    if (now - last_message > mqtt_interval_ms)
    {
        last_message = now;

        // Les led et la batteries sont branchés tous les deux sur le pin 32
        // du coup, lire la valeur de batterie fait freeze la batterie
        // Battery level
        // dtostrf(Get_Battery_Voltage(), 5, 2, buff);
        // client.publish("esp32/battery", buff);

        // Track Read
        Track_Read();
        sensor_v = static_cast<int>(sensorValue[3]);
        char const *n_char = std::to_string(sensor_v).c_str();
        client.publish("esp32/track", n_char);

        // Ultrasonic Data
        dtostrf(Get_Sonar(), 5, 2, ultrasonic_buff);
        client.publish("esp32/sonar", ultrasonic_buff);

        // Photosensitive Data
        dtostrf(Get_Photosensitive(), 5, 2, ultrasonic_buff);
        client.publish("esp32/light", ultrasonic_buff);


    }







  



}



void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {





        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(mqttId, mqttUser, mqttPass))
        {
            Serial.println("connected");
            // Subscribe
            client.subscribe("esp32/output");

        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}



void loopTask_Camera(void *pvParameters)
{
    while (1)
    {
        char size_buf[12];
        WiFiClient wf_client = server_Camera.available(); // listen for incoming clients
        if (wf_client)
        { // if you get a client
            Serial.println("Camera_Server connected to a client.");
            if (wf_client.connected())
            {
                camera_fb_t *fb = NULL;
                wf_client.write("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: multipart/x-mixed-replace; boundary=" STREAM_CONTENT_BOUNDARY "\r\n");
                while (wf_client.connected())
                { // loop while the client's connected
                    if (videoFlag == 1)
                    {
                        fb = esp_camera_fb_get();
                        if (fb != NULL)
                        {
                            wf_client.write("\r\n--" STREAM_CONTENT_BOUNDARY "\r\n");
                            wf_client.write("Content-Type: image/jpeg\r\nContent-Length: ");
                            sprintf(size_buf, "%d\r\n\r\n", fb->len);
                            wf_client.write(size_buf);
                            wf_client.write(fb->buf, fb->len);

                            // uint8_t slen[4];
                            // slen[0] = fb->len >> 0;
                            // slen[1] = fb->len >> 8;
                            // slen[2] = fb->len >> 16;
                            // slen[3] = fb->len >> 24;
                            // wf_client.write(slen, 4);
                            // wf_client.write(fb->buf, fb->len);
                            // Serial.println("Camera send");
                            //esp_camera_fb_return(fb);
                        }
                    }
                }
                // close the connection:
                wf_client.stop();
                Serial.println("Camera Client Disconnected.");
                // ESP.restart();
            }
        }
    }
}