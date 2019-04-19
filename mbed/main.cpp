// ----------------------------------------------------------------------------
// Copyright 2016-2018 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#ifndef MBED_TEST_MODE

#include "mbed.h"
#include "mbed_trace.h"
#include "simple-mbed-cloud-client.h"
#include "LittleFileSystem.h"
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"
#include "LSM6DSLSensor.h"
#include "lis3mdl_class.h"
#include "VL53L0X.h"

#define WITH_HTTP		// WITH_HTTP | WITH_HTTPS

#if defined(WITH_HTTPS)
#include "https_request.h"
#else
#include "http_request.h"
#endif

#include "http_response.h"


static DevI2C devI2c(PB_11,PB_10);
static HTS221Sensor sen_hum_temp(&devI2c);
static LPS22HBSensor sen_press_temp(&devI2c);
static LSM6DSLSensor sen_acc_gyro(&devI2c,LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW,PD_11); // low address
static LIS3MDL sen_mag(&devI2c);
static DigitalOut shutdown_pin(PC_6);
static VL53L0X sen_distance(&devI2c, &shutdown_pin, PC_7);

#define SENSORS_POLL_INTERVAL 5.0
#define SEND_ALL_SENSORS

#if 0
const char SSL_CA_PEM[] =  "-----BEGIN CERTIFICATE-----\n"
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
    "rqXRfboQnoZsG4q5WTP468SQvvG5\n"
    "-----END CERTIFICATE-----\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
    "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n"
    "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n"
    "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"
    "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n"
    "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n"
    "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n"
    "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n"
    "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n"
    "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n"
    "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n"
    "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n"
    "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n"
    "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n"
    "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n"
    "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n"
    "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n"
    "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n"
    "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n"
    "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n"
    "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n"
    "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n"
    "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n"
    "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n"
    "-----END CERTIFICATE-----\n";
#else
const char SSL_CA_PEM[] =  \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIICiTCCAg+gAwIBAgIQH0evqmIAcFBUTAGem2OZKjAKBggqhkjOPQQDAzCBhTEL\n" \
  "MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n" \
  "BxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMT\n" \
  "IkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDgwMzA2MDAw\n" \
  "MDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdy\n" \
  "ZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09N\n" \
  "T0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlv\n" \
  "biBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQDR3svdcmCFYX7deSR\n" \
  "FtSrYpn1PlILBs5BAH+X4QokPB0BBO490o0JlwzgdeT6+3eKKvUDYEs2ixYjFq0J\n" \
  "cfRK9ChQtP6IHG4/bC8vCVlbpVsLM5niwz2J+Wos77LTBumjQjBAMB0GA1UdDgQW\n" \
  "BBR1cacZSBm8nZ3qQUfflMRId5nTeTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/\n" \
  "BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjEA7wNbeqy3eApyt4jf/7VGFAkK+qDm\n" \
  "fQjGGoe9GKhzvSbKYAydzpmfz1wPMOG+FDHqAjAU9JM8SaczepBGR7NjfRObTrdv\n" \
  "GDeAU/7dIOA1mjbRxwG55tzd8/8dLDoWV9mSOdY=\n" \
  "-----END CERTIFICATE-----\n";
#endif

void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());
    printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\nBody (%lu bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

// An event queue is a very useful structure to debounce information between contexts (e.g. ISR and normal threads)
// This is great because things such as network operations are illegal in ISR, so updating a resource in a button's fall() function is not allowed
EventQueue eventQueue;

// Default network interface object. Don't forget to change the WiFi SSID/password in mbed_app.json if you're using WiFi.
NetworkInterface *net;

int num_of_pelion = 0;

// Default block device
BlockDevice* bd = BlockDevice::get_default_instance();
SlicingBlockDevice sd(bd, 0, 2*1024*1024);
LittleFileSystem fs("fs");

// Default User button for GET example
InterruptIn button(BUTTON1);
// Default LED to use for PUT/POST example
DigitalOut led(LED1);

// Declaring pointers for access to Pelion Client resources outside of main()
MbedCloudClientResource *res_button;
MbedCloudClientResource *res_led;

#if 0
//#ifdef SEND_ALL_SENSORS
// Temperature reading from microcontroller
AnalogIn adc_temp(ADC_TEMP);
// Voltage reference reading from microcontroller
AnalogIn adc_vref(ADC_VREF);
#endif

// Additional resources for sensor readings
#if 0
MbedCloudClientResource *res_humidity;
MbedCloudClientResource *res_temperature;
//#ifdef SEND_ALL_SENSORS
MbedCloudClientResource *res_pressure;
MbedCloudClientResource *res_temperature2;
MbedCloudClientResource *res_magnometer_x;
MbedCloudClientResource *res_magnometer_y;
MbedCloudClientResource *res_magnometer_z;
MbedCloudClientResource *res_accelerometer_x;
MbedCloudClientResource *res_accelerometer_y;
MbedCloudClientResource *res_accelerometer_z;
MbedCloudClientResource *res_gyroscope_x;
MbedCloudClientResource *res_gyroscope_y;
MbedCloudClientResource *res_gyroscope_z;
MbedCloudClientResource *res_distance;
MbedCloudClientResource *res_adc_temp;
MbedCloudClientResource *res_adc_voltage;
#endif /* SEND_ALL_SENSORS */

MbedCloudClientResource *res_popup_iot;;

// When the device is registered, this variable will be used to access various useful information, like device ID etc.
static const ConnectorClientEndpointInfo* endpointInfo;

/**
 * PUT handler
 * @param resource The resource that triggered the callback
 * @param newValue Updated value for the resource
 */
void put_callback(MbedCloudClientResource *resource, m2m::String newValue) {
    printf("*** PUT received, new value: %s                             \n", newValue.c_str());
    led = atoi(newValue.c_str());
}

/**
 * POST handler
 * @param resource The resource that triggered the callback
 * @param buffer If a body was passed to the POST function, this contains the data.
 *               Note that the buffer is deallocated after leaving this function, so copy it if you need it longer.
 * @param size Size of the body
 */
void post_callback(MbedCloudClientResource *resource, const uint8_t *buffer, uint16_t size) {
    printf("*** POST received (length %u). Payload: ", size);
    for (size_t ix = 0; ix < size; ix++) {
        printf("%02x ", buffer[ix]);
    }
    printf("\n");
}

/**
 * Button function triggered by the physical button press.
 */
void button_press() {
    int v = res_button->get_value_int() + 1;
    res_button->set_value(v);
    printf("*** Button clicked %d times                                 \n", v);
}

/**
 * Notification callback handler
 * @param resource The resource that triggered the callback
 * @param status The delivery status of the notification
 */
void button_callback(MbedCloudClientResource *resource, const NoticationDeliveryStatus status) {
    printf("*** Button notification, status %s (%d)                     \n", MbedCloudClientResource::delivery_status_to_string(status), status);
}

/**
 * Registration callback handler
 * @param endpoint Information about the registered endpoint such as the name (so you can find it back in portal)
 */
void registered(const ConnectorClientEndpointInfo *endpoint) {
    printf("Registered to Pelion Device Management. Endpoint Name: %s\n", endpoint->internal_endpoint_name.c_str());
    endpointInfo = endpoint;
}

/**
 * Initialize sensors
 */
void sensors_init() {
    uint8_t id1, id2, id3, id4;

    printf ("\nSensors configuration:\n");
    // Initialize sensors
#if 0
    sen_hum_temp.init(NULL);
    sen_press_temp.init(NULL);
    sen_acc_gyro.init(NULL);
    sen_mag.init(NULL);
    sen_distance.init_sensor(VL53L0X_DEFAULT_ADDRESS);

    /// Call sensors enable routines
    sen_hum_temp.enable();
    sen_press_temp.enable();
    sen_acc_gyro.enable_x();
    sen_acc_gyro.enable_g();

    sen_hum_temp.read_id(&id1);
    sen_press_temp.read_id(&id2);
    sen_mag.read_id(&id3);
    sen_acc_gyro.read_id(&id4);

    printf("HTS221  humidity & temperature    = 0x%X\n", id1);
    printf("LPS22HB pressure & temperature    = 0x%X\n", id2);
    printf("LIS3MDL magnetometer              = 0x%X\n", id3);
    printf("LSM6DSL accelerometer & gyroscope = 0x%X\n", id4);

    printf("\n"); ;
#endif
}

/**
 * Update sensors and report their values.
 * This function is called periodically.
 */
void sensors_update() {
#if 0
    float temp1_value, temp2_value, temp3_value, humid_value, pressure_value, volt_value = 0.0;
    int32_t m_axes[3], a_axes[3], g_axes[3];
    uint32_t distance_value, distance_reading;

    sen_hum_temp.get_humidity(&humid_value);
    sen_hum_temp.get_temperature(&temp1_value);
    sen_press_temp.get_pressure(&pressure_value);
    sen_press_temp.get_temperature(&temp2_value);
    sen_mag.get_m_axes(m_axes);
    sen_acc_gyro.get_x_axes(a_axes);
    sen_acc_gyro.get_g_axes(g_axes);
    distance_reading = sen_distance.get_distance(&distance_value);
    temp3_value = adc_temp.read()*100;
    volt_value = adc_vref.read();

    float mag_x =  (double)m_axes[0] / 1000.0, mag_y  = (double)m_axes[1] / 1000.0, mag_z  = (double)m_axes[2] / 1000.0;
    float acc_x =  (double)a_axes[0] / 1000.0, acc_y  = (double)a_axes[1] / 1000.0, acc_z  = (double)a_axes[2] / 1000.0;
    float gyro_x = (double)g_axes[0] / 1000.0, gyro_y = (double)g_axes[1] / 1000.0, gyro_z = (double)g_axes[2] / 1000.0;
#endif

/* hyhwang
    printf("                                                             \n");
    printf("ADC temp:     %5.4f C,  vref:      %5.4f V         \n", temp3_value, volt_value);
    printf("HTS221 temp:  %7.3f C,  humidity: %7.2f %%         \n", temp1_value, humid_value);
    printf("LPS22HB temp: %7.3f C,  pressure: %7.2f mbar       \n", temp2_value, pressure_value);
    printf("LIS3MDL mag:  %7.3f x, %7.3f y, %7.3f z [gauss]      \n", mag_x, mag_y, mag_z);
    printf("LSM6DSL acc:  %7.3f x, %7.3f y, %7.3f z [g]          \n", acc_x, acc_y, acc_z);
    printf("LSM6DSL gyro: %7.3f x, %7.3f y, %7.3f z [dps]        \n", gyro_x, gyro_y, gyro_z);
    if (distance_reading == VL53L0X_ERROR_NONE) {
        printf("VL53L0X dist: %7ld mm\n", distance_value);
    } else {
        printf("VL53L0X dist:        --       \n");
        distance_value = 999;
    }

    printf("\r\033[8A");
*/

    if (endpointInfo) {
#if 0
        res_humidity->set_value(humid_value);
        res_temperature->set_value(temp1_value);
//#ifdef SEND_ALL_SENSORS
        res_pressure->set_value(pressure_value);
        res_temperature2->set_value(temp2_value);
        res_magnometer_x->set_value(mag_x);
        res_magnometer_y->set_value(mag_y);
        res_magnometer_z->set_value(mag_z);
        res_accelerometer_x->set_value(acc_x);
        res_accelerometer_y->set_value(acc_y);
        res_accelerometer_z->set_value(acc_z);
        res_gyroscope_x->set_value(gyro_x);
        res_gyroscope_y->set_value(gyro_y);
        res_gyroscope_z->set_value(gyro_z);
        res_distance->set_value((int)distance_value);
        res_adc_temp->set_value(temp3_value);
        res_adc_voltage->set_value(volt_value);
#endif /* SEND_ALL_SENSORS */


#if defined(WITH_HTTPS)
        HttpsRequest* request = new HttpsRequest(net, SSL_CA_PEM, HTTP_GET, "https://dweet.io/get/latest/dweet/for/popup-iot");
        HttpResponse* response = request->send();
        if (!response) {
            printf("HttpRequest failed (error code %d)\n", request->get_error());
            return;
        }
#else
        const char body[] = "{\"hello\":\"world\"}";
        HttpRequest* request = new HttpRequest(net, HTTP_GET, "http://www.tric.kr/popup-iot/json/");
        request->set_header("Content-Type", "application/json");
        HttpResponse* response = request->send(body, strlen(body));
#endif
        if (!response) {
            printf("HttpRequest failed (error code %d)\n", request->get_error());
            return;
        }
        printf("#%05d mbed-cloud: %d %s", num_of_pelion, response->get_status_code(), response->get_body_as_string().c_str());
        res_popup_iot->set_value(response->get_body_as_string().c_str());
	num_of_pelion += 1;
        delete request; // also clears out the response
    }
}

int main(void) {
    printf("\nStarting Simple Pelion Device Management Client example\n");

    int storage_status = fs.mount(&sd);
    if (storage_status != 0) {
        printf("Storage mounting failed.\n");
    }
#if USE_BUTTON == 1
    // If the User button is pressed ons start, then format storage.
    bool btn_pressed = (button.read() == MBED_CONF_APP_BUTTON_PRESSED_STATE);
    if (btn_pressed) {
        printf("User button is pushed on start...\n");
    }
#else
    bool btn_pressed = FALSE;
#endif /* USE_BUTTON */

    if (storage_status || btn_pressed) {
        printf("Formatting the storage...\n");
        int storage_status = StorageHelper::format(&fs, &sd);
        if (storage_status != 0) {
            printf("ERROR: Failed to reformat the storage (%d).\n", storage_status);
        }
    } else {
        printf("You can hold the user button during boot to format the storage and change the device identity.\n");
    }

    sensors_init();

    // Connect to the internet (DHCP is expected to be on)
    printf("Connecting to the network using Wifi...\n");
    net = NetworkInterface::get_default_instance();

    nsapi_error_t net_status = -1;
    for (int tries = 0; tries < 3; tries++) {
        net_status = net->connect();
        if (net_status == NSAPI_ERROR_OK) {
            break;
        } else {
            printf("Unable to connect to network. Retrying...\n");
        }
    }

    if (net_status != NSAPI_ERROR_OK) {
        printf("ERROR: Connecting to the network failed (%d)!\n", net_status);
        return -1;
    }

    printf("Connected to the network successfully. IP address: %s\n", net->get_ip_address());

    printf("Initializing Pelion Device Management Client...\n");

    // SimpleMbedCloudClient handles registering over LwM2M to Pelion DM
    SimpleMbedCloudClient client(net, bd, &fs);
    int client_status = client.init();
    if (client_status != 0) {
        printf("ERROR: Pelion Client initialization failed (%d)\n", client_status);
        return -1;
    }

    // Creating resources, which can be written or read from the cloud
    res_button = client.create_resource("3200/0/5501", "Button Count");
    res_button->set_value(0);
    res_button->methods(M2MMethod::GET);
    res_button->observable(true);
    res_button->attach_notification_callback(button_callback);

    // Sensor resources
#if 0
    res_temperature = client.create_resource("3303/0/5700", "Temperature HTS221 (C)");
    res_temperature->set_value(0);
    res_temperature->methods(M2MMethod::GET);
    res_temperature->observable(true);

    res_humidity = client.create_resource("3304/0/5700", "Humidity");
    res_humidity->set_value(0);
    res_humidity->methods(M2MMethod::GET);
    res_humidity->observable(true);

//#ifdef SEND_ALL_SENSORS
    res_temperature2 = client.create_resource("3303/1/5700", "Temperature LPS22HB (C)");
    res_temperature2->set_value(0);
    res_temperature2->methods(M2MMethod::GET);
    res_temperature2->observable(true);

    res_adc_temp = client.create_resource("3303/2/5700", "Temperature ADC (C)");
    res_adc_temp->set_value(0);
    res_adc_temp->methods(M2MMethod::GET);
    res_adc_temp->observable(true);

    res_accelerometer_x = client.create_resource("3313/0/5702", "Accelerometer X");
    res_accelerometer_x->set_value(0);
    res_accelerometer_x->methods(M2MMethod::GET);
    res_accelerometer_x->observable(true);

    res_accelerometer_y = client.create_resource("3313/0/5703", "Accelerometer Y");
    res_accelerometer_y->set_value(0);
    res_accelerometer_y->methods(M2MMethod::GET);
    res_accelerometer_y->observable(true);

    res_accelerometer_z = client.create_resource("3313/0/5704", "Accelerometer Z");
    res_accelerometer_z->set_value(0);
    res_accelerometer_z->methods(M2MMethod::GET);
    res_accelerometer_z->observable(true);

    res_magnometer_x = client.create_resource("3314/0/5702", "Magnometer X");
    res_magnometer_x->set_value(0);
    res_magnometer_x->methods(M2MMethod::GET);
    res_magnometer_x->observable(true);

    res_magnometer_y = client.create_resource("3314/0/5703", "Magnometer Y");
    res_magnometer_y->set_value(0);
    res_magnometer_y->methods(M2MMethod::GET);
    res_magnometer_y->observable(true);

    res_magnometer_z = client.create_resource("3314/0/5704", "Magnometer Z");
    res_magnometer_z->set_value(0);
    res_magnometer_z->methods(M2MMethod::GET);
    res_magnometer_z->observable(true);

    res_gyroscope_x = client.create_resource("3334/0/5702", "Gyroscope X");
    res_gyroscope_x->set_value(0);
    res_gyroscope_x->methods(M2MMethod::GET);
    res_gyroscope_x->observable(true);

    res_gyroscope_y = client.create_resource("3334/0/5703", "Gyroscope Y");
    res_gyroscope_y->set_value(0);
    res_gyroscope_y->methods(M2MMethod::GET);
    res_gyroscope_y->observable(true);

    res_gyroscope_z = client.create_resource("3334/0/5704", "Gyroscope Z");
    res_gyroscope_z->set_value(0);
    res_gyroscope_z->methods(M2MMethod::GET);
    res_gyroscope_z->observable(true);

    res_adc_voltage = client.create_resource("3316/0/5700", "Voltage");
    res_adc_voltage->set_value(0);
    res_adc_voltage->methods(M2MMethod::GET);
    res_adc_voltage->observable(true);

    res_pressure = client.create_resource("3323/0/5700", "Pressure");
    res_pressure->set_value(0);
    res_pressure->methods(M2MMethod::GET);
    res_pressure->observable(true);

    res_distance = client.create_resource("3330/0/5700", "Distance");
    res_distance->set_value((float)999.9);
    res_distance->methods(M2MMethod::GET);
    res_distance->observable(true);

    res_led = client.create_resource("3201/0/5853", "LED State");
    res_led->set_value(1);
    res_led->methods(M2MMethod::GET | M2MMethod::PUT);
    res_led->attach_put_callback(put_callback);
#endif /* SEND_ALL_SENSORS */

    res_popup_iot = client.create_resource("5555/0/6666", "POPUP-IoT Sensor");
    res_popup_iot->set_value("{}");
    res_popup_iot->methods(M2MMethod::GET);
    res_popup_iot->observable(true);

    printf("Initialized Pelion Client. Registering...\n");

    // Callback that fires when registering is complete
    client.on_registered(&registered);

    // Register with Pelion DM
    client.register_and_connect();

    int i = 600; // wait up 60 seconds before attaching sensors and button events
    while (i-- > 0 && !client.is_client_registered()) {
        wait_ms(100);
    }

    button.fall(eventQueue.event(&button_press));

    // The timer fires on an interrupt context, but debounces it to the eventqueue, so it's safe to do network operations
    Ticker timer;
    timer.attach(eventQueue.event(&sensors_update), SENSORS_POLL_INTERVAL);

    // You can easily run the eventQueue in a separate thread if required
    eventQueue.dispatch_forever();
}

#endif
