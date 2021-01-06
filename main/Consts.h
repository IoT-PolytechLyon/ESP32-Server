/* NB : 
 * - WIFI credentials are located in the HttpServer.h file. Same goes for the NodeExpress server IP.
 * - NFC Reader must be plugged into UART pin
 * - Motion sensor must be plugged into D4-D5
 * - LED must be plugged into D2-D3
 * - ...
 */


/* consts */ // TODO : put them into a header file.
#define NFC_READ_DELAY                             1000   // ms
#define NFC_READ_DELAY_AFTER_BADGING_ATTEMPT       3000   // ms
#define SERVER_TIMEOUT_TIME                        3000   // ms
#define PIR_TIMEOUT_TIME                           5000   // ms
#define PIR_DETECTION_DELAY                        50     // ms
#define LED_UPDATE_DELAY                           50     // ms

#define MOTION_SENSOR_D4                           15     // pin number
#define LED_D2                                     14     // pin number
#define LED_D3                                     32     // pin number
