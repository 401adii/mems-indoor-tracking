#ifndef WIFI_AP_H_
#define WIFI_AP_H_

#define WIFI_AP_MAX_CONNECTION 4
#define WIFI_AP_CHANNEL 1

void wifi_ap_init(const char* ssid, const char* password);

#endif // WIFI_AP_H_