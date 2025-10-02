#ifndef COMM_H
#define COMM_H

#include "esp_now.h"

// Initialize communication with callbacks
void comm_init(esp_now_send_cb_t send_cb, esp_now_recv_cb_t recv_cb);

// Adds a peer to communicate with
void comm_add_peer(uint8_t *mac_addr);

#endif