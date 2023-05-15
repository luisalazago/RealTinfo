/*******************************************************************************
 *
 *  pMLusrConf.h -   Manifest Constants and Types for concurrent access to a
 *                   circular buffer modelling a message queue
 *
 *   Notes:          User defined according to application
 *
 *******************************************************************************/

/***( Manifest constants for fser-defined queuing system  )********************/

#define BUFSIZE 8         /* number of slots in queues */
#define NUM_QUEUES 8      /* number of queues */
#define MICROPHONE 0      /* queue 0: microphone */
#define CAMERA 1          /* queue 1: camera */
#define SENSOR_MOVEMENT 2 /* queue 2: sensor movement */
#define CONTROLLER 3      /* queue 3: controller */
#define ADMIN 4           /* queue 4: admin */
#define SERVER 5          /* queue 5: server */
#define DATA_BASE 6       /* queue 6: data base */
#define USER 7            /* queue 7: user  */
#define CLOUD 8           /* queue 8: cloud (final device) */
/***( User-defined message structure )*****************************************/

typedef struct {
  int signal;
  int value;
  char device[20];
  char info[150];
} msg_t;

/*typedef struct {
  int signal;
  //int value;
  char *dispositivo;
  char *segnal;
} msg_t;*/

/***( User-defined signals
 * )****************************************************/

typedef enum {
	sendInfo,
	move,
	busy,
	warning
} TO_DEVICES;

typedef enum {
  answer,
  info1,
  errorFound,
	isInfo,
	td,
	lt,
	requestInfo,
	tiempo
} TO_SERVER_BD;

typedef enum{
  sendInfoBD,
	ansRequest,
	dropData
} TO_BD_CLOUD;

/***( User-defined EFSM states
 * )************************************************/
typedef enum {
	Device,
	turnOn,
	turnOff,
	error,
  move_detected
} PROCESO_DEVICES_STATES;

typedef enum {
  Comunicaciones_dispocontro,
  Apagar,
  Prender,
  RecibirInfo
} PROCESO_CONTROLADOR_STATES;

typedef enum { Comunicaciones_acciones } PROCESO_ACCIONES_STATES;

typedef enum { Comunicaciones_admin } PROCESO_ADMIN_STATES;

typedef enum {
  Comunicaciones_serBDNube,
  RecibirRes,
  Respuesta,
  DevolverInfo,
  DevolverErrores
} PROCESO_SERBDNUBE_STATES;

typedef enum { Comunicaciones_bd } PROCESO_BD_STATES;
