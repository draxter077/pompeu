#include "web_server.h"
#include "uart_bridge.h"
#include <string.h>
#include <stdio.h>
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

static const char *html_page = // HTML da página no root
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <title>DDCP 2026</title>\n"
"    <style>\n"
"        body {\n"
"            font-family: Arial, sans-serif;\n"
"            background-color: #f4f4f9;\n"
"            margin: 0;\n"
"            padding: 10px;\n"
"            text-align: center;\n"
"        }\n"
"        header {\n"
"            display: flex;\n"
"            justify-content: space-between;\n"
"            align-items: center;\n"
"            background: #333;\n"
"            color: #fff;\n"
"            padding: 10px 20px;\n"
"            border-radius: 8px;\n"
"        }\n"
"        .header-section {\n"
"            font-size: 14px;\n"
"            text-align: left;\n"
"        }\n"
"        .header-center {\n"
"            font-size: 20px;\n"
"            font-weight: bold;\n"
"        }\n"
"        .container {\n"
"            background: #fff;\n"
"            margin-top: 15px;\n"
"            padding: 15px;\n"
"            border-radius: 8px;\n"
"            box-shadow: 0 0 10px rgba(0,0,0,0.1);\n"
"        }\n"
"        .joystick-container {\n"
"            width: 200px;\n"
"            height: 200px;\n"
"            background: #ddd;\n"
"            border-radius: 50%;\n"
"            margin: 20px auto;\n"
"            position: relative;\n"
"            touch-action: none;\n"
"        }\n"
"        .joystick-knob {\n"
"            width: 70px;\n"
"            height: 70px;\n"
"            background: #007bff;\n"
"            border-radius: 50%;\n"
"            position: absolute;\n"
"            top: 65px;\n"
"            left: 65px;\n"
"            cursor: pointer;\n"
"        }\n"
"        button {\n"
"            padding: 10px 15px;\n"
"            margin: 5px;\n"
"            font-size: 14px;\n"
"            cursor: pointer;\n"
"            border: none;\n"
"            border-radius: 5px;\n"
"            background: #007bff;\n"
"            color: white;\n"
"        }\n"
"        .danger {\n"
"            background:red;\n"
"        }\n"
"        input[type=\"text\"] {\n"
"            padding: 8px;\n"
"            width: 60%;\n"
"            max-width: 250px;\n"
"            margin-right: 5px;\n"
"        }\n"
"    </style>\n"
"</head>\n"
"<body>\n"
"    <header>\n"
"        <div class=\"header-section\" id=\"clock-box\">Carregando relógio...</div>\n" // Cabeçalho da página com relógio
"        <div class=\"header-center\">DDCP 2026</div>\n"
"    </header>\n"
"\n"
"    <div class=\"container\">\n" // Representação de comando joystick
"        <h3>Controle Manual (Joystick)</h3>\n"
"        <div class=\"joystick-container\" id=\"joystick\">\n"
"            <div class=\"joystick-knob\" id=\"knob\"></div>\n"
"        </div>\n"
"    </div>\n"
"\n"
"    <div class=\"container\">\n" // Botão de parada de emergência
"        <button class=\"danger\" style=\"font-size: 18px; width: 80%; padding: 15px;\" onclick=\"sendCmd('CMD:EMERGENCY')\">PARADA DE EMERGÊNCIA</button>\n"
"    </div>\n"
"\n"
"    <div class=\"container\">\n" // Campo para escrita no display
"        <h3>Display</h3>\n"
"        <input type=\"text\" id=\"display-text\" maxlength=\"20\" placeholder=\"Digite aqui\">\n"
"        <button onclick=\"sendDisplay()\">Enviar Display</button>\n"
"    </div>\n"
"\n"
"    <script>\n"
"        function updateClock() {\n" // Função para atualizar relógio do cabeçalho
"            const now = new Date();\n"
"            const timeStr = now.toTimeString().split(' ')[0];\n"
"            const days = ['Domingo', 'Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado'];\n"
"            const dayStr = days[now.getDay()];\n"
"            document.getElementById('clock-box').innerHTML = timeStr + '<br>' + dayStr;\n"
"        }\n"
"        setInterval(updateClock, 1000);\n"
"\n"
"        function sendCmd(cmd) {\n" // Função interface geral WEB-UART
"            fetch('/cmd?msg=' + encodeURIComponent(cmd));\n"
"        }\n"
"\n"
"        const joystick = document.getElementById('joystick');\n"
"        const knob = document.getElementById('knob');\n"
"        let dragging = false;\n"
"        const maxD = 100;\n"
"\n"
"        function handleMove(clientX, clientY) {\n" // Função para medir movimentação do joystick
"            const rect = joystick.getBoundingClientRect();\n"
"            const centerX = rect.left + rect.width/2;\n"
"            const centerY = rect.top + rect.height/2;\n"
"            let dx = clientX - centerX;\n" // Calcula diferença no eixo X
"            let dy = clientY - centerY;\n" // Calcula diferença no eixo Y
"            if(dx <= maxD && dx >= -maxD && dy <= maxD && dy >= -maxD){\n"
"               knob.style.transform = `translate(${dx}px, ${dy}px)`;\n"
"               sendCmd('M ' + (dx/maxD).toFixed(3) + ' ' + (dy/maxD).toFixed(3));\n" // Define comunicação sobre motores a ser enviada
"            }\n"
"        }\n"
"\n"
"        knob.addEventListener('mousedown', (e) => { dragging = true; });\n" // EventListener para mouses
"        window.addEventListener('mousemove', (e) => { if (dragging) handleMove(e.clientX, e.clientY); });\n"
"        window.addEventListener('mouseup', () => {\n"
"            if (dragging) {\n"
"                dragging = false;\n"
"                knob.style.transform = 'translate(0px, 0px)';\n"
"                sendCmd('M 0 0');\n"
"            }\n"
"        });\n"
"\n"
"        knob.addEventListener('touchstart', (e) => { dragging = true; });\n" // EventListener para touchscreen
"        window.addEventListener('touchmove', (e) => { if (dragging) handleMove(e.touches[0].clientX, e.touches[0].clientY); });\n"
"        window.addEventListener('touchend', () => {\n"
"            if (dragging) {\n"
"                dragging = false;\n"
"                knob.style.transform = 'translate(0px, 0px)';\n"
"                sendCmd('M 0 0 0');\n"
"            }\n"
"        });\n"
"\n"
"        function sendDisplay() {\n" // Envia comunicações sobre o texto do display
"            const text = document.getElementById('display-text').value;\n"
"            sendCmd('D ' + text);\n"
"        }\n"
"    </script>\n"
"</body>\n"
"</html>";

static esp_err_t root_get_handler(httpd_req_t *req){ // Handler do root
    httpd_resp_set_type(req, "text/html"); // Define tipo do envio
    httpd_resp_send(req, html_page, strlen(html_page)); // Envia
    return ESP_OK;
}

static esp_err_t cmd_get_handler(httpd_req_t *req){ // Handler de envio de comandos
    char buf[128];
    size_t buf_len = sizeof(buf);
    
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK){
        char param[64];
        if (httpd_query_key_value(buf, "msg", param, sizeof(param)) == ESP_OK) uart_send_command(param); // Envia a comunicação por UART
    }
    
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static const httpd_uri_t root = { // Objeto caminho /
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t cmd = { // Objeto caminho /cmd
    .uri       = "/cmd",
    .method    = HTTP_GET,
    .handler   = cmd_get_handler,
    .user_ctx  = NULL
};

void start_webserver(void){ // Inicialização dos recursos para o protocolo HTTP
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;

    if (httpd_start(&server, &config) == ESP_OK){
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &cmd);
    }
}

void wifi_init_softap(void){ // Inicializa e configura o ponto de acesso WIFI
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap(); // Alocações iniciais de recursos

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = { // Objeto de configurações do WIFI
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = 1,
            .password = WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start()); // Inicializações efetivas do recurso
}