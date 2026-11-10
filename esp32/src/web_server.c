#include "web_server.h"
#include "uart_bridge.h"
#include <string.h>
#include <stdio.h>
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

// Página HTML completa contendo a interface do usuário (Cabeçalho, Relógio, Posição, Joystick, Localizações, Display e Buzzer com bloqueio de 25s)
static const char *html_page = 
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
"        button.danger {\n"
"            background: #dc3545;\n"
"        }\n"
"        button:disabled {\n"
"            background: #ccc;\n"
"            cursor: not-allowed;\n"
"        }\n"
"        input[type=\"text\"] {\n"
"            padding: 8px;\n"
"            width: 60%;\n"
"            max-width: 250px;\n"
"            margin-right: 5px;\n"
"        }\n"
"        .loc-list {\n"
"            margin-top: 10px;\n"
"            display: flex;\n"
"            flex-direction: column;\n"
"            align-items: center;\n"
"        }\n"
"    </style>\n"
"</head>\n"
"<body>\n"
"    <header>\n"
"        <div class=\"header-section\" id=\"clock-box\">Carregando relógio...</div>\n"
"        <div class=\"header-center\">DDCP 2026</div>\n"
"        <div class=\"header-section\">Pos: <span id=\"pos-box\">0,0</span></div>\n"
"    </header>\n"
"\n"
"    <div class=\"container\">\n"
"        <h3>Controle Manual (Joystick)</h3>\n"
"        <div class=\"joystick-container\" id=\"joystick\">\n"
"            <div class=\"joystick-knob\" id=\"knob\"></div>\n"
"        </div>\n"
"    </div>\n"
"\n"
"    <div class=\"container\">\n"
"        <h3>Gestão de Localizações</h3>\n"
"        <button onclick=\"sendCmd('GOTO 0 0')\">Retornar para (0,0)</button>\n"
"        <div style=\"margin-top: 10px;\">\n"
"            <input type=\"text\" id=\"loc-name\" maxlength=\"20\" placeholder=\"Nome do local (max 20)\">\n"
"            <button onclick=\"saveLocation()\">Salvar Local</button>\n"
"        </div>\n"
"        <div class=\"loc-list\" id=\"loc-container\"></div>\n"
"    </div>\n"
"\n"
"    <div class=\"container\">\n"
"        <h3>Parada de Emergência</h3>\n"
"        <button class=\"danger\" style=\"font-size: 18px; width: 80%; padding: 15px;\" onclick=\"sendCmd('CMD:EMERGENCY')\">PARADA DE EMERGÊNCIA</button>\n"
"    </div>\n"
"\n"
"    <div class=\"container\">\n"
"        <h3>Display</h3>\n"
"        <input type=\"text\" id=\"display-text\" maxlength=\"20\" placeholder=\"Texto para o display (max 20)\">\n"
"        <button onclick=\"sendDisplay()\">Enviar Display</button>\n"
"    </div>\n"
"\n"
"    <div class=\"container\">\n"
"        <h3>Buzzer</h3>\n"
"        <button id=\"buzzer-btn\" onclick=\"triggerBuzzer()\">Ativar Buzzer</button>\n"
"    </div>\n"
"\n"
"    <script>\n"
"        function updateClock() {\n"
"            const now = new Date();\n"
"            const timeStr = now.toTimeString().split(' ')[0];\n"
"            const days = ['Domingo', 'Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado'];\n"
"            const dayStr = days[now.getDay()];\n"
"            document.getElementById('clock-box').innerHTML = timeStr + '<br>' + dayStr;\n"
"        }\n"
"        setInterval(updateClock, 1000);\n"
"\n"
"        function updateCoords() {\n"
"            fetch('/get_coords').then(res => res.text()).then(data => {\n"
"                document.getElementById('pos-box').innerText = data;\n"
"            });\n"
"        }\n"
"        setInterval(updateCoords, 500);\n"
"\n"
"        function sendCmd(cmd) {\n"
"            fetch('/cmd?msg=' + encodeURIComponent(cmd));\n"
"        }\n"
"\n"
"        const joystick = document.getElementById('joystick');\n"
"        const knob = document.getElementById('knob');\n"
"        let dragging = false;\n"
"        const maxDist = 65;\n"
"\n"
"        function handleMove(clientX, clientY) {\n"
"            const rect = joystick.getBoundingClientRect();\n"
"            const centerX = rect.left + rect.width / 2;\n"
"            const centerY = rect.top + rect.height / 2;\n"
"            let dx = clientX - centerX;\n"
"            let dy = clientY - centerY;\n"
"            const dist = Math.sqrt(dx * dx + dy * dy);\n"
"            if (dist > maxDist) {\n"
"                dx = (dx / dist) * maxDist;\n"
"                dy = (dy / dist) * maxDist;\n"
"            }\n"
"            knob.style.transform = `translate(${dx}px, ${dy}px)`;\n"
"            const normX = (dx / maxDist).toFixed(2);\n"
"            const normY = (-dy / maxDist).toFixed(2);\n"
"            const angle = Math.atan2(dx, -dy) * (180 / Math.PI);\n"
"            sendCmd('M ' + normX + ' ' + normY + ' ' + Math.round(angle));\n"
"        }\n"
"\n"
"        knob.addEventListener('mousedown', (e) => { dragging = true; });\n"
"        window.addEventListener('mousemove', (e) => { if (dragging) handleMove(e.clientX, e.clientY); });\n"
"        window.addEventListener('mouseup', () => {\n"
"            if (dragging) {\n"
"                dragging = false;\n"
"                knob.style.transform = 'translate(0px, 0px)';\n"
"                sendCmd('M 0 0 0');\n"
"            }\n"
"        });\n"
"\n"
"        knob.addEventListener('touchstart', (e) => { dragging = true; });\n"
"        window.addEventListener('touchmove', (e) => { if (dragging) handleMove(e.touches[0].clientX, e.touches[0].clientY); });\n"
"        window.addEventListener('touchend', () => {\n"
"            if (dragging) {\n"
"                dragging = false;\n"
"                knob.style.transform = 'translate(0px, 0px)';\n"
"                sendCmd('M 0 0 0');\n"
"            }\n"
"        });\n"
"\n"
"        function saveLocation() {\n"
"            const name = document.getElementById('loc-name').value;\n"
"            if (!name) return;\n"
"            fetch('/get_coords').then(res => res.text()).then(coords => {\n"
"                const container = document.getElementById('loc-container');\n"
"                const btn = document.createElement('button');\n"
"                btn.innerText = name + ' (' + coords + ')';\n"
"                btn.onclick = function() { sendCmd('GOTO ' + coords); };\n"
"                container.appendChild(btn);\n"
"                document.getElementById('loc-name').value = '';\n"
"            });\n"
"        }\n"
"\n"
"        function sendDisplay() {\n"
"            const text = document.getElementById('display-text').value;\n"
"            sendCmd('D ' + text);\n"
"        }\n"
"\n"
"        function triggerBuzzer() {\n"
"            sendCmd('B');\n"
"            const btn = document.getElementById('buzzer-btn');\n"
"            btn.disabled = true;\n"
"            let timeLeft = 25;\n"
"            btn.innerText = 'Aguarde ' + timeLeft + 's';\n"
"            let timer = setInterval(() => {\n"
"                timeLeft--;\n"
"                if (timeLeft > 0) {\n"
"                    btn.innerText = 'Aguarde ' + timeLeft + 's';\n"
"                } else {\n"
"                    clearInterval(timer);\n"
"                    btn.disabled = false;\n"
"                    btn.innerText = 'Ativar Buzzer';\n"
"                }\n"
"            }, 1000);\n"
"        }\n"
"    </script>\n"
"</body>\n"
"</html>";

// Handler para a rota raiz "/"
static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

// Handler para a rota de comandos "/cmd"
static esp_err_t cmd_get_handler(httpd_req_t *req) {
    char buf[128];
    size_t buf_len = sizeof(buf);
    
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
        char param[64];
        if (httpd_query_key_value(buf, "msg", param, sizeof(param)) == ESP_OK) {
            uart_send_command(param);
        }
    }
    
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

// Handler para a rota de consulta de coordenadas "/get_coords"
static esp_err_t coords_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, latest_coords, strlen(latest_coords));
    return ESP_OK;
}

// Estruturas de registro das URIs
static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t cmd = {
    .uri       = "/cmd",
    .method    = HTTP_GET,
    .handler   = cmd_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t coords = {
    .uri       = "/get_coords",
    .method    = HTTP_GET,
    .handler   = coords_get_handler,
    .user_ctx  = NULL
};

// Função para iniciar o servidor web
httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &cmd);
        httpd_register_uri_handler(server, &coords);
    }

    return server;
}

// Função para parar o servidor web
void stop_webserver(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}