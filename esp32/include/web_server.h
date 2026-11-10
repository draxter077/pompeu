#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h"

// Protótipos das funções do módulo de servidor web e páginas da interface
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

#endif