#include    <stdio.h>
#include    <string.h>
#include    <errno.h>

/* SDL Creation */

static void *pServer( void *arg );

int main( void  )
{
   pthread_t server_tid;
   pthread_t nube_tid;
   pthread_t admin_tid;
   pthread_t devices_tid;

   pthread_create ( &server_tid, NULL, pServer, NULL );
   pthread_join ( server_tid, NULL );
   return( 0 );
}

static void *pServer( void *arg )
{
   CONTROLLER_STATES state, state_next;
   msg_t InMsg;
   for( ; ; ; )
   {
      InMsg = recive( &(queue [CONTROLLER_Q]) );
      switch( state )
      {
         case nube:
            switch( InMsg.signal ):
            {
               case sendInfo: break;
               case getInfo: break;
               case sendInforme: break;
               case dropInfo: break;
               default: break;
            }
            break;
         case admin:
            switch( InMsg.signal):
            {
               case sendError: break;
               case getNotification: break;
               default: break;
            }
            break;
         case devices:
            switch( InMsg.signal ):
            {
               case sendInfo: break;
               case sendError: break;
               default: break;
            }
            break;
         default: break;
      }
   }
   return( NULL );
}