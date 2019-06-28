// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_netif.h"
#include "lwip/netif.h"

// LWIP netif specific network stack configuration
struct esp_netif_netstack_config {
    esp_netif_netstack_base_config_t base;
    err_t (*init_fn)(struct netif*);
    void (*input_fn)(struct netif *netif, void *buffer, size_t len, void *eb);
};

struct esp_netif_api_msg_s;

typedef int (*esp_netif_api_fn)(struct esp_netif_api_msg_s *msg);

typedef struct esp_netif_api_msg_s {
    int type;  /**< The first field MUST be int */
    int ret;
    esp_netif_api_fn api_fn;
    esp_netif_t *esp_netif;
    void    *data;
} esp_netif_api_msg_t;


typedef struct esp_netif_dns_param_s {
    esp_netif_dns_type_t dns_type;
    esp_netif_dns_info_t *dns_info;
} esp_netif_dns_param_t;

typedef struct esp_netif_ip_lost_timer_s {
    bool timer_running;
} esp_netif_ip_lost_timer_t;


#define ESP_NETIF_TRHEAD_SAFE 1
#define ESP_NETIF_IPC_LOCAL   0
#define ESP_NETIF_IPC_REMOTE  1

#define ESP_NETIF_IPC_CALL(_if, _data, _fn) do {\
    esp_netif_api_msg_t msg;\
    if (tcpip_initialized == false) {\
        ESP_LOGE(TAG, "esp_netif is not initialized!");\
        abort();\
    }\
    memset(&msg, 0, sizeof(msg));\
    msg.esp_netif = (_if);\
    msg.data     = (void*)(_data);\
    msg.api_fn   = (_fn);\
    if (ESP_NETIF_IPC_REMOTE == esp_netif_ipc_check(&msg)) {\
        ESP_LOGD(TAG, "check: remote, if=%p fn=%p\n", (_if), (_fn));\
        return msg.ret;\
    } else {\
        ESP_LOGD(TAG, "check: local, if=%p fn=%p\n", (_if), (_fn));\
    }\
} while(0)
