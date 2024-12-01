#include "esp_wifi.h"
#include "esp_log.h"
#include "network.h"
#include "esp_websocket_client.h"

#include "taskmgr.h"
#include "wifi_creds.h"

char * getCurrentTaskName(){
    return pcTaskGetName(NULL);
}

#define TAG "NET"

static void connect_to_websocket();

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data){
    ESP_LOGI("main", "WiFi handler running in %s\n", getCurrentTaskName());
    if(event_id == WIFI_EVENT_STA_START) {
        printf("WIFI CONNECTING....\n");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("WiFi CONNECTED\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("WiFi lost connection\n");
        printf("Retrying to Connect...\n");
        esp_wifi_connect();
    }
    else if (event_id == IP_EVENT_STA_GOT_IP){
        printf("Wifi got IP...\n\n");
         connect_to_websocket();
    } else{
        printf("Unknown wifi related event %ld\n", event_id);
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static esp_websocket_client_handle_t ws_c;


void send_rpc(uint32_t cmd, const char *data, int len){
    char* buff = malloc(len+4);
    *(uint32_t *)buff = cmd;
    memcpy(buff + 4, data, len);
    esp_websocket_client_send_bin(ws_c, buff, len+4, portMAX_DELAY);
    free(buff);
}


static void handle_rpc_data(uint32_t cmd, const char *buff, int len){
    if(cmd == IC("PRGS")){
        taskmgr_update_progs_list(buff, len);
    } else if (cmd == IC("RUN ")){
        ESP_LOG_BUFFER_HEXDUMP(TAG, buff, len, ESP_LOG_INFO);
        taskmgr_run_js(buff, buff + strlen(buff) + 1);
    }
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "WS handler running in %s\n", getCurrentTaskName());
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_BEGIN:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_BEGIN");
            break;
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
            esp_websocket_client_send_bin(ws_c, "PRGS", 4, portMAX_DELAY);
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
            log_error_if_nonzero("HTTP status code",  data->error_handle.esp_ws_handshake_status_code);
            if (data->error_handle.error_type == WEBSOCKET_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", data->error_handle.esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", data->error_handle.esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  data->error_handle.esp_transport_sock_errno);
            }
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
            ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
            if (data->op_code == 0x2) { // Opcode 0x2 indicates binary data
                if(data->data_len < 4){
                    ESP_LOGE(TAG, "Invalid command");
                    ESP_LOG_BUFFER_HEX("Received binary data", data->data_ptr, data->data_len);
                } else{
                    handle_rpc_data(*(uint32_t*)data->data_ptr, data->data_ptr + 4, data->data_len - 4);
                }
                
            } else if (data->op_code == 0x08 && data->data_len == 2) {
                ESP_LOGW(TAG, "Received closed message with code=%d", 256 * data->data_ptr[0] + data->data_ptr[1]);
            } else {
                ESP_LOGW(TAG, "Received=%.*s\n\n", data->data_len, (char *)data->data_ptr);
            }


            ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);

            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
            log_error_if_nonzero("HTTP status code",  data->error_handle.esp_ws_handshake_status_code);
            if (data->error_handle.error_type == WEBSOCKET_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", data->error_handle.esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", data->error_handle.esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  data->error_handle.esp_transport_sock_errno);
            }
            break;
        case WEBSOCKET_EVENT_FINISH:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_FINISH");
            connect_to_websocket();
            break;
    }
}

static void connect_to_websocket(){
    esp_websocket_client_config_t websocket_cfg = {
        .uri = "ws://192.168.2.228:8080/device",
    };
    ws_c = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_client_destroy_on_exit(ws_c);
    esp_websocket_register_events(ws_c, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)ws_c);
    esp_websocket_client_start(ws_c);

}

void connect_to_wifi(){
    ESP_ERROR_CHECK(esp_netif_init());
    /*esp_netif_obj * netif = */esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
            .sta={
                    .ssid = WIFI_SSID,
                    .password = WIFI_PASSWORD,
            }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_wifi_connect());

}