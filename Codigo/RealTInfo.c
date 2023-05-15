#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "phtrdsMsgLyr.h"
/* SDL Creation */

static void *pServer(void *arg);
static void *pCamera(void *arg);
static void *pMicro(void *arg);
static void *pMoveSensor(void *arg);
static void *pController(void *arg);
static void *pDatabase(void *arg);
static void *pAdmin(void *arg);
static void *pNube(void *arg);

int main(void) {
  pthread_t server_tid;
  pthread_t camera_tid;
  pthread_t micro_tid;
  pthread_t moveSensor_tid;
  pthread_t controller_tid;
  pthread_t database_tid;
  pthread_t admin_tid;
  pthread_t nube_tid;

  /* Create Queues */
  initialiseQueues();

  /* Create Threads */
  pthread_create(&server_tid, NULL, pServer, NULL);
  pthread_create(&camera_tid, NULL, pCamera, NULL);
  pthread_create(&micro_tid, NULL, pMicro, NULL);
  pthread_create(&moveSensor_tid, NULL, pMoveSensor, NULL);
  pthread_create(&controller_tid, NULL, pController, NULL);
  pthread_create(&database_tid, NULL, pDatabase, NULL);
  pthread_create(&admin_tid, NULL, pAdmin, NULL);
  pthread_create(&nube_tid, NULL, pNube, NULL);

  /* Wait for the threads to finish */
  pthread_join(moveSensor_tid, NULL);
  pthread_join(micro_tid, NULL);
  pthread_join(camera_tid, NULL);
  pthread_join(controller_tid, NULL);
  pthread_join(server_tid, NULL);
  pthread_join(database_tid, NULL);
  pthread_join(admin_tid, NULL);
  pthread_join(nube_tid, NULL);
  return (0);
}

static void *pMoveSensor(void *arg) {
  PROCESO_DEVICES_STATES state, state_next;
  msg_t InMsg, OutMsg;
  state_next = Device;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_MOVEMENT]));
    switch (state) 
		{
	    case Device:
	      switch (InMsg.signal)
				{
		      case move_detected:
		        state_next = Device;
		        OutMsg.signal = move;
		        OutMsg.value = InMsg.value;
		        sendMessage(&(queue[CONTROLLER]), OutMsg);
		        break;
		      case error:
		        state_next = Device;
		        OutMsg.signal = warning;
		        strcpy(OutMsg.device, "Sensor");
		        strcpy(OutMsg.info, InMsg.info);
		        sendMessage(&(queue[CONTROLLER]), OutMsg);
		        break;
		      default:
		        break;
	      }
	      break;
	    default:
	      break;
    }
  }
  return NULL;
}

static void *pMicro(void *arg) {
  PROCESO_DEVICES_STATES state, state_next;
  msg_t InMsg, OutMsg;
  int recording = 0;

  state_next = Device;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[MICROPHONE]));
    switch (state)
		{
	    case Device:
	      switch (InMsg.signal) 
				{
		      case turnOn:
		        state_next = Device;
		        recording = 1;
		        break;
		      case turnOff:
		        state_next = Device;
		        recording = 0;
		        OutMsg.signal = sendInfo;
		        OutMsg.value = recording;
		        sendMessage(&(queue[CONTROLLER]), OutMsg);
		        break;
		      case error:
		        state_next = Device;
		        strcpy(OutMsg.device, "Microphone");
		        strcpy(OutMsg.info, InMsg.info);
		        sendMessage(&(queue[CONTROLLER]), OutMsg);
		        break;
		      default:
		        break;
	      }
	      break;
	    default:
	      break;
    }
  }
  return NULL;
}

static void *pCamera(void *arg) {
  PROCESO_DEVICES_STATES state, state_next;
  msg_t InMsg, OutMsg;
  int recording = 0;

  state_next = Device;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[CAMERA]));
    switch (state)
		{
	    case Device:
	      switch (InMsg.signal)
				{
		      case turnOn:
		        state_next = Device;
		        recording = 1;
		        break;
		      case turnOff:
		        state_next = Device;
		        recording = 0;
		        OutMsg.signal = sendInfo;
		        OutMsg.value = recording;
		        sendMessage(&(queue[CONTROLLER]), OutMsg);
		        break;
		      case error:
		        state_next = Device;
		        strcpy(OutMsg.device, "Camera");
		        strcpy(OutMsg.info, InMsg.info);
		        sendMessage(&(queue[CONTROLLER]), OutMsg);
		        break;
		      default:
		        break;
	      }
	      break;
	    default:
	      break;
    }
  }
  return NULL;
}

static void *pController(void *arg) {
  int Ti = 0, b = 0; // Timer
  PROCESO_CONTROLADOR_STATES state, state_next;
  msg_t InMsg, OutMsg;

  state_next = Comunicaciones_dispocontro; // Initialize first state
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[CONTROLLER]));
    switch (state)
		{
	    case Comunicaciones_dispocontro:
	      switch (InMsg.signal)
				{
		      case warning:
		        OutMsg.signal = warning;
		        OutMsg.value = InMsg.value;
		        strcpy(OutMsg.device, InMsg.device);
		        strcpy(OutMsg.info, InMsg.info);
		        state_next = Comunicaciones_dispocontro;
		        sendMessage(&(queue[SERVER]), OutMsg);
		        break;
		      case move:
		        if (InMsg.value == 1) {
		          state_next = Prender;
		          OutMsg.signal = busy;
		          OutMsg.value = b;
		          sendMessage(&(queue[CONTROLLER]), OutMsg);
		          Ti = 0;
		        } else {
		          if (++Ti == 30) {
		            state_next = Apagar;
		            OutMsg.signal = busy;
		            OutMsg.value = b;
		            sendMessage(&(queue[CONTROLLER]), OutMsg);
		          } else
		            state_next = Comunicaciones_dispocontro;
		        }
		        break;
		      default:
		        break;
	      }
	      break;
	    case Prender:
	      if (InMsg.signal == busy) {
	        if (InMsg.value == 1)
	          state_next = Comunicaciones_dispocontro;
	        else {
	          b = 1;
	          state_next = Comunicaciones_dispocontro;
	          OutMsg.signal = turnOn;
	          sendMessage(&(queue[MICROPHONE]), OutMsg);
	          sendMessage(&(queue[CAMERA]), OutMsg);
	        }
	      } else
	        sendMessage(&(queue[CONTROLLER]), InMsg);
	      break;
	    case Apagar:
	      if (InMsg.signal == busy) {
	        state_next = RecibirInfo;
	        b = 0;
	        OutMsg.signal = turnOff;
	        OutMsg.value = 0;
	        sendMessage(&(queue[MICROPHONE]), OutMsg);
	        sendMessage(&(queue[CAMERA]), OutMsg);
	      } else
	        sendMessage(&(queue[CONTROLLER]), InMsg);
	      break;
	    case RecibirInfo:
	      switch (InMsg.signal)
				{
		      case sendInfo:
		        state_next = Comunicaciones_dispocontro;
		        OutMsg.signal = sendInfo;
		        OutMsg.value = InMsg.value;
		        sendMessage(&(queue[SERVER]), OutMsg);
		        break;
		      default:
		        break;
		    }
	      break;
	    default:
	      break;
    }
  }
}

static void *pServer(void *arg) {
  int flag;
  PROCESO_ACCIONES_STATES state, state_next;
  msg_t InMsg, OutMsg;

  flag = 0;
  state_next = Comunicaciones_acciones;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SERVER]));
    switch (state)  
    {
      case Comunicaciones_acciones:
        switch (InMsg.signal) 
        {
          case requestInfo:
            OutMsg.signal = isInfo;
            OutMsg.value = InMsg.value;
            sendMessage(&(queue[DATA_BASE]), OutMsg);
            break;
          case errorFound:
            OutMsg.signal = errorFound;
            if (InMsg.value == 1)
              strcpy(OutMsg.info, "DATA NOT FOUND");
            else if (InMsg.value == 2)
              strcpy(OutMsg.info, "ERROR FILTERING");
            else
              strcpy(OutMsg.info, "UNKNOW ERROR");
            sendMessage(&(queue[ADMIN]), OutMsg);
            sendMessage(&(queue[USER]), OutMsg);
            break;
          case tiempo:
            if (InMsg.value >= 83 && InMsg.value < 90) {
              OutMsg.signal = td;
              OutMsg.value = 0;
              sendMessage(&(queue[USER]), OutMsg);
            } else {
              if (InMsg.value == 90) {
                OutMsg.signal = lt;
                OutMsg.value = 1;
                sendMessage(&(queue[DATA_BASE]), OutMsg);
              } else {
                OutMsg.signal = tiempo;
                OutMsg.value = InMsg.value;
                sendMessage(&(queue[DATA_BASE]), OutMsg);
              }
            }
            break;
          case warning:
            OutMsg.signal = warning;
            OutMsg.value = InMsg.value;
            strcpy(OutMsg.device, InMsg.device);
            strcpy(OutMsg.info, InMsg.info);
            sendMessage(&(queue[ADMIN]), OutMsg);
            break;
          case sendInfo:
            if (flag == 0) {
              ++flag;
              OutMsg.signal = tiempo;
              OutMsg.value = 0;
              sendMessage(&(queue[DATA_BASE]), OutMsg);
            }
            OutMsg.signal = sendInfo;
            OutMsg.value = InMsg.value;
            sendMessage(&(queue[DATA_BASE]), OutMsg);
            break;
          default:
            break;
          }
        state_next = Comunicaciones_acciones;
        break;
      default:
        break;
    }
  }
  return (NULL);
}

static void *pAdmin(void *arg) {
  PROCESO_ADMIN_STATES state, state_next;
  msg_t InMsg, OutMsg;

  state_next = Comunicaciones_admin;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SERVER]));
    switch (state) 
    {
      case Comunicaciones_admin:
        switch (InMsg.signal) 
        {
          case warning:
            printf("\n");
            printf("Warning:\n");
            printf("%s in the device: %s\n", InMsg.info, InMsg.device);
            break;
          case errorFound:
            printf("\n");
            printf("Error in the user's request:\n");
            printf("%s\n", InMsg.info);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}

static void *pNube(void *arg) {
  PROCESO_SERBDNUBE_STATES state, state_next;
  msg_t InMsg, OutMsg;
  state_next = Comunicaciones_serBDNube;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[CLOUD]));
    switch (state)
    {
      case Comunicaciones_serBDNube:
        switch (InMsg.signal)
        {
          case sendInfo:
            OutMsg.signal = sendInfo;
            OutMsg.value = InMsg.value;
            sendMessage(&(queue[DATA_BASE]), OutMsg);
            break;
          case td:
            OutMsg.signal = td;
            strcpy(OutMsg.info, "Hi, everyone! Its time to drop");
            sendMessage(&(queue[USER]), OutMsg);
            break;
          case requestInfo:
            OutMsg.signal = requestInfo;
            OutMsg.value = InMsg.value;
            sendMessage(&(queue[SERVER]), OutMsg);
            state_next = RecibirRes;
            break;
          case lt:
            OutMsg.signal = lt;
            strcpy(OutMsg.info,
                   "Hi everyone! The time has come to erase everything.");
            sendMessage(&(queue[DATA_BASE]), OutMsg);
            break;
          case tiempo:
            OutMsg.signal = tiempo;
            OutMsg.value = InMsg.value;
            sendMessage(&(queue[DATA_BASE]), OutMsg);
            break;
          default:
            break;
        }
        break;
      case DevolverInfo:
        switch (InMsg.signal)
        {
          case info1:
            state_next = Comunicaciones_serBDNube;
            OutMsg.signal = ansRequest;
            strcpy(OutMsg.info, InMsg.info);
            sendMessage(&(queue[USER]), OutMsg);
            break;
          default:
            break;
        }
        break;
      case DevolverErrores:
        switch (InMsg.signal)
        {
          case errorFound:
            OutMsg.signal = errorFound;
            strcpy(OutMsg.info, InMsg.info);
            sendMessage(&(queue[SERVER]), OutMsg);
            OutMsg.signal = ansRequest;
            sendMessage(&(queue[USER]), OutMsg);
            state_next = Comunicaciones_serBDNube;
            break;
          default:
            break;
        }
        break;
      case RecibirRes:
        switch (InMsg.signal) 
        {
          case isInfo:
            OutMsg.signal = isInfo;
            OutMsg.value = InMsg.value;
            sendMessage(&(queue[DATA_BASE]), OutMsg);
            state_next = Respuesta;
            break;
          default:
            break;
        }  
        break;
      case Respuesta:
        switch (InMsg.signal) 
        {
          case answer:
            if (InMsg.value == 1) 
            {
              state_next = DevolverInfo;
            } else {
              state_next = DevolverErrores;
            }
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  return (NULL);
}

static void *pDatabase(void *arg) {
  PROCESO_BD_STATES state, state_next;
  msg_t InMsg, OutMsg;
  int tt;

  state_next = Comunicaciones_bd;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[DATA_BASE]));
    switch (state) 
    {
      case Comunicaciones_bd:
        switch (InMsg.signal) 
        {
          case sendInfo:
            printf("Saved successfully!.\n");
            break;
          case dropData:
            printf("Data erased successfully.\n");
            break;
          case isInfo:
            if (strcmp(InMsg.info, "get") || strcmp(InMsg.info, "read")) 
            {
              OutMsg.signal = answer;
              OutMsg.value = 1;
              sendMessage(&(queue[CLOUD]), OutMsg);
              OutMsg.signal = info1;
              strcpy(InMsg.info,
                     "Aquí iría la información, si tuviéramos una db. unu");
              sendMessage(&(queue[CLOUD]), OutMsg);
            } else {
              OutMsg.signal = answer;
              OutMsg.value = 0;
              sendMessage(&(queue[CLOUD]), OutMsg);
              OutMsg.signal = errorFound;
              strcpy(InMsg.info, "Aquí iría el error, si supieramos cuál es. unu");
              sendMessage(&(queue[CLOUD]), OutMsg);
            }
          case tiempo:
            tt = InMsg.value + 1;
            OutMsg.signal = tiempo;
            OutMsg.value = tt;
            sendMessage(&(queue[CLOUD]), OutMsg);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}