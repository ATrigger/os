//
// Created by kamenev on 10.01.16.
//

#ifndef POLL_EVENT_UTILS_H
#define POLL_EVENT_UTILS_H
#include <inttypes.h>
#include <bits/errno.h>
#include <time.h>
#include <string>
#include <cerrno>
#include "handle.h"
bool
str_to_uint16(const char *str, uint16_t *res);
const std::string currentDateTime();
int getSocketError(const handle& fd);
const std::string currentTime();


#endif //POLL_EVENT_UTILS_H
