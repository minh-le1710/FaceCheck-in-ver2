# FaceCheck-in-FreeRTOS-ESP32
FaceCheck-in FreeRTOS ESP32 Use FreeRTOS and 1.54 inch IPS LCD

## RTOS Priority Task
| Task Name                 | Priority         | Core Affinity | Frequency / Trigger       | Notes                     |
|---------------------------|---------------------|---------------|--------------------------|---------------------------|
| MQTT Handler              | 4                 | Core 1        | On MQTT message (IRQ)    | Adafruit IO callback      |
| Buzzer & LED Control      | 3        | Core 0        | On event (semaphore)     | Non-blocking, short burst |
| Quote Update            | 2       | Core 0        | Every 1 hour (timer)        | Fetch daily quote via API    |
| Time & Weather Update     | 2               | Core 0        | Every 1 minute (timer)      | HTTP + NTP                |
| Display Update            | 1                | Core 1        | Every 1 s                   | Draw clock & status       |

## Dashboard
320*240 Resolution

![Face Checkin Demo](face-checkin-2/240320%20(240%20x%20240%20px)%20(1).png)
