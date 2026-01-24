#include <Arduino.h>
#include "melty_config.h"


#ifndef WATCHDOG_HEADER_GUARD
#define WATCHDOG_HEADER_GUARD

#ifdef ENABLE_WATCHDOG
#include "Watchdog_t4.h"
WDT_T4<WDT3> wdt;
#endif


void service_watchdog() {
    #ifdef ENABLE_WATCHDOG
    wdt.feed();
    #endif
}

void my_watchdog_callback() {
    Serial.println("FEED THE DOG SOON, OR RESET!");
}

void init_watchdog() {
    #ifdef ENABLE_WATCHDOG
    WDT_timings_t config;
    config.trigger = 1000;
    config.timeout = WATCH_DOG_TIMEOUT_MS;
    config.callback = my_watchdog_callback;
    Serial.print("Watchdog begin");
    Serial.println(WATCH_DOG_TIMEOUT_MS);
    wdt.begin(config);
    #endif
}


#endif