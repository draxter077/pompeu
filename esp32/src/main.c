#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/uart.h"

#define UART_NUM         UART_NUM_2
#define TXD_PIN          (17)
#define RXD_PIN          (16)
#define BUF_SIZE         (1024)

#define WIFI_SSID        "DCPP2026_Robot"
#define WIFI_PASS        "12345678"
#define MAX_STA_CONN     4

static char latest_coords[64] = "0,0";

static const char *html_page = 
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <title>DCPP2026 Control</title>\n"
"    <style>\n"
"        body {\n"
"            font-family: sans-serif;\n"
"            text-align: center;\n"
"            background: #121212;\n"
"            color: #fff;\n"
"            margin: 0;\n"
"            padding: 20px;\n"
"        }\n"
"        h2 { color: #00adb5; }\n"
"        button {\n"
"            background: #00adb5;\n"
"            color: #fff;\n"
"            border: none;\n"
"            padding: 12px 20px;\n"
"            margin: 6px;\n"
"            font-size: 16px;\n"
"            border-radius: 5px;\n"
"            cursor: pointer;\n"
"        }\n"
"        button:active { background: #007b83; }\n"
"        input, select {\n"
"            padding: 10px;\n"
"            margin: 6px;\n"
"            font-size: 16px;\n"
"            border-radius: 5px;\n"
"            border: 1px solid #444;\n"
"            background: #222;\n"
"            color: #fff;\n"
"            width: 80%;\n"
"            max-width: 300px;\n"
"        }\n"
"        .box {\n"
"            background: #1e1e1e;\n"
"            padding: 15px;\n"
"            margin: 10px auto;\n"
"            border-radius: 8px;\n"
"            max-width: 350px;\n"
"            box-shadow: 0 4px 6px rgba(0,0,0,0.3);\n"
"        }\n"
"        #currentCoordDisplay {\n"
"            font-size: 20px;\n"
"            color: #ff2e63;\n"
"            font-weight: bold;\n"
"            margin: 10px 0;\n"
"        }\n"
"    </style>\n"
"</head>\n"
"<body>\n"
"    <h2>DCPP2026 - Controle</h2>\n"
"    \n"
"    <div class=\"box\">\n"
"        <h3>Posição Atual</h3>\n"
"        <div id=\"currentCoordDisplay\">X: 0, Y: 0</div>\n"
"    </div>\n"
"    \n"
"    <div class=\"box\">\n"
"        <h3>Movimentação</h3>\n"
"        <button onclick=\"sendCmd('CMD:F')\">Frente</button><br>\n"
"        <button onclick=\"sendCmd('CMD:L')\">Esquerda</button>\n"
"        <button onclick=\"sendCmd('CMD:S')\">Parar</button>\n"
"        <button onclick=\"sendCmd('CMD:R')\">Direita</button><br>\n"
"        <button onclick=\"sendCmd('CMD:B')\">Trás</button><br>\n"
"        <button onclick=\"sendCmd('CMD:SPIN_L')\">Girar Esquerda (Eixo)</button>\n"
"        <button onclick=\"sendCmd('CMD:SPIN_R')\">Girar Direita (Eixo)</button>\n"
"    </div>\n"
"    \n"
"    <div class=\"box\">\n"
"        <h3>Localização</h3>\n"
"        <input type=\"text\" id=\"locName\" placeholder=\"Nome do Local (ex: Ponto A)\"><br>\n"
"        <button onclick=\"saveLocation()\">Salvar Localização Atual</button><br>\n"
"        <select id=\"locSelect\">\n"
"            <option value=\"\">Selecione um local salvo</option>\n"
"        </select><br>\n"
"        <button onclick=\"sendSelectedLoc()\">Ir para Localização</button>\n"
"    </div>\n"
"    \n"
"    <div class=\"box\">\n"
"        <h3>Display & Buzzer</h3>\n"
"        <input type=\"text\" id=\"displayText\" placeholder=\"Texto para o Display\"><br>\n"
"        <button onclick=\"sendText()\">Enviar para Display</button><br>\n"
"        <button onclick=\"sendCmd('BUZZER:ON')\">Tocar Buzzer</button>\n"
"    </div>\n"
"    \n"
"    <script>\n"
"        function sendCmd(cmd) {\n"
"            fetch('/cmd?data=' + encodeURIComponent(cmd));\n"
"        }\n"
"        \n"
"        function sendText() {\n"
"            let t = document.getElementById('displayText').value;\n"
"            fetch('/cmd?data=' + encodeURIComponent('DISP:' + t));\n"
"        }\n"
"        \n"
"        function saveLocation() {\n"
"            let name = document.getElementById('locName').value;\n"
"            if (!name) {\n"
"                alert('Preencha o nome do local!');\n"
"                return;\n"
"            }\n"
"            fetch('/save_loc?name=' + encodeURIComponent(name))\n"
"            .then(response => response.text())\n"
"            .then(coord => {\n"
"                if(coord && coord !== 'ERROR') {\n"
"                    let select = document.getElementById('locSelect');\n"
"                    let option = document.createElement('option');\n"
"                    option.value = 'GOTO:' + coord;\n"
"                    option.text = name + ' (' + coord + ')';\n"
"                    select.appendChild(option);\n"
"                    document.getElementById('locName').value = '';\n"
"                } else {\n"
"                    alert('Erro ao obter coordenada do STM32');\n"
"                }\n"
"            });\n"
"        }\n"
"        \n"
"        function sendSelectedLoc() {\n"
"            let l = document.getElementById('locSelect').value;\n"
"            if (!l) {\n"
"                alert('Selecione um local!');\n"
"                return;\n"
"            }\n"
"            fetch('/cmd?data=' + encodeURIComponent(l));\n"
"        }\n"
"        \n"
"        function updateCoords() {\n"
"            fetch('/get_coords')\n"
"            .then(response => response.text())\n"
"            .then(coord => {\n"
"                if(coord) {\n"
"                    document.getElementById('currentCoordDisplay').innerText = 'X, Y: ' + coord;\n"
"                }\n"
"            });\n"
"        }\n"
"        \n"
"        setInterval(updateCoords, 1000);\n"
"    </script>\n"
"</body>\n"
"</html>\n";

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

static esp_err_t cmd_get_handler(httpd_req_t *req) {
    char buf[128];
    char param_val[100];

    if (httpd_req_get_url_query_len(req) > 0) {
        if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
            if (httpd_query_key_value(buf, "data", param_val, sizeof(param_val)) == ESP_OK) {
                uart_write_bytes(UART_NUM, param_val, strlen(param_val));
                uart_write_bytes(UART_NUM, "\n", 1);
            }
        }
    }
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t save_loc_handler(httpd_req_t *req) {
    uart_write_bytes(UART_NUM, "GET_POS\n", 8);

    vTaskDelay(pdMS_TO_TICKS(100));

    httpd_resp_send(req, latest_coords, strlen(latest_coords));
    return ESP_OK;
}

static esp_err_t get_coords_handler(httpd_req_t *req) {
    httpd_resp_send(req, latest_coords, strlen(latest_coords));
    return ESP_OK;
}

static void uart_listener_task(void *pvParameters) {
    char *line_buf = (char *)malloc(BUF_SIZE);
    int idx = 0;
    while (1) {
        uint8_t c;
        int len = uart_read_bytes(UART_NUM, &c, 1, pdMS_TO_TICKS(50));
        if (len > 0) {
            if (c == '\n' || c == '\r') {
                if (idx > 0) {
                    line_buf[idx] = '\0';
                    if (strncmp(line_buf, "POS:", 4) == 0) {
                        snprintf(latest_coords, sizeof(latest_coords), "%s", line_buf + 4);
                    }
                    idx = 0;
                }
            } else {
                if (idx < BUF_SIZE - 1) {
                    line_buf[idx++] = c;
                }
            }
        }
    }
    free(line_buf);
}

static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri      = "/",
            .method   = HTTP_GET,
            .handler  = root_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t cmd_uri = {
            .uri      = "/cmd",
            .method   = HTTP_GET,
            .handler  = cmd_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &cmd_uri);

        httpd_uri_t save_loc_uri = {
            .uri      = "/save_loc",
            .method   = HTTP_GET,
            .handler  = save_loc_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &save_loc_uri);

        httpd_uri_t get_coords_uri = {
            .uri      = "/get_coords",
            .method   = HTTP_GET,
            .handler  = get_coords_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &get_coords_uri);
    }
    return server;
}

static void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = 1,
            .password = WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(esp_nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    wifi_init_softap();
    start_webserver();

    xTaskCreate(uart_listener_task, "uart_listener_task", 4096, NULL, 5, NULL);
}