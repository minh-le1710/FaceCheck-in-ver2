# FaceCheck-in-FreeRTOS-ESP32
FaceCheck-in FreeRTOS ESP32 Use FreeRTOS and 1.54 inch IPS LCD

## RTOS Priority Task
| Task Name                 | Priority         | Core Affinity | Frequency / Trigger       | Notes                     |
|---------------------------|---------------------|---------------|--------------------------|---------------------------|
| Time & Weather Update     | 2               | Core 0        | Every 1 minute (timer)      | HTTP + NTP                |
| MQTT Handler              | 4                 | Core 1        | On MQTT message (IRQ)    | Adafruit IO callback      |
| Display Update            | 1                | Core 1        | Every 1 s                   | Draw clock & status       |
| Buzzer & LED Control      | 3        | Core 0        | On event (semaphore)     | Non-blocking, short burst |

