#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#include "uart_bridge.h"
#include "web_server.h"

void app_main(void) {
    nvs_flash_init(); // Inicialização da memória Flash
    wifi_init_softap(); // Inicializa ponto de acesso WIFI
    uart_bridge_init(); // Inicializa e configura comunicação UART
    start_webserver(); // Configura protocolo HTTP
}