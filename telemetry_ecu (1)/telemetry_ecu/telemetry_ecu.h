// OJO REPASAR TODO

// CAN Telemetría config
//#define CAN_CS 7
#define CAN_CS 43
//#define CAN_INT 6
#define CAN_INT 37
#define CAN_KBPS CAN_250KBPS

// Delays
#define DELAY_CONFIG 100

// Comprobaciones y flags: 1 listo, 0 en proceso todavía
int ecu_master = 1;
int RTD_all = 1;

// ---------- IDs de los buses CAN ----------
// IDs CAN Telemetría Generales
INT32U ID_RTD_all = 0x80;

// IDs CAN Telemetría ECU_Telemetría
INT32U ID_ok_telemetria = 0xA0;
INT32U ID_ack_telemetria = 0x30;
