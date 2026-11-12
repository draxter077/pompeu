#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h"

#define WIFI_SSID       "DCPP"
#define WIFI_PASS       "rambo2026"
#define MAX_STA_CONN    4

void start_webserver(void); // Função para inicializar recurso
void wifi_init_softap(void) // Função para configurar ponto de acesso WIFI

#endif