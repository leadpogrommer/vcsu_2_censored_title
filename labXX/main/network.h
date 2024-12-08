#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define IC(x) (*((uint32_t *)(x)))

void connect_to_wifi();
void send_rpc(uint32_t cmd, const char *data, int len);

#ifdef __cplusplus
}
#endif
