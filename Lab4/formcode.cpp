/* Rev:$Revision: 1.1 $ */
/******************************************************************************
 * Copyright 1998-2008 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing@Netburner.com
 *   for more information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected
 *   with the use of this program.
 *
 *   NetBurner, Inc
 *   5405 Morehouse Drive
 *   San Diego Ca, 92121
 *   http://www.netburner.com
 *
 *****************************************************************************/
#include "predef.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <constants.h>
#include <basictypes.h>
#include <buffers.h>
#include <htmlfiles.h>
#include <http.h>
#include <utils.h>
#include <iosys.h>
#include <system.h>
#include <limits.h>

#define FIELD_EMPTY 0
#define FIELD_NOT_FOUND -1

/*-------------------------------------------------------------------
 * The FlashForm example demonstrates three different capabilities:
 * 1. Dynamic HTML
 * 2. HTML Form processing
 * 3. User Flash parameter storage.
 *
 * DYNAMIC HTML
 * -------------
 * Look in the file html/INDEX.HTM under this project. Inside that
 * file you will notice two HTML comments:
 *     <!--FUNCTIONCALL DoMessageName -->
 *     <!--FUNCTIONCALL DoMessageBody -->
 *
 * When the this HTML page is requested by a web browser, the NetBurner
 * web server will begin streaming out the data. When a FUNCTIONCALL tag
 * is encountered, the web server will execute the specified function.
 * The function is passed a file descitptor to the socket and can then
 * send whatever dynamic content to the web browser. In this example the
 * functions are:
 *    void DoMessageName(int sock, PCSTR url);
 *    void DoMessageBody(int sock, PCSTR url);
 *
 * These functions use the following I/O calls to send the dynamic
 * data:
 *
 *    int  write(int fd, const char * buf, int nbytes); // Write any data
 *    void writestring(int fd, const char * str);     // Write a null terminated string
 *    void writesafestring(int fd, const char * str); // Write a string escaping any special HTML chars
 *
 * That is all there is to it!
 *
 * FORM PROCESSING
 * ---------------
 * Whenever an HTTP POST operation occurs, the system will call a
 * http_posthandler function. This function takes the form:
 *
 *    int DoPost(int sock, char *url, char *pData, char *rxBuffer )
 *
 * The NetBurner system library has an empty function as a placeholder. To use
 * FORMs in your application you will need to write your own function and
 * REGISTER it for use. The Registration function is:
 *
 * http_posthandler * SetNewPostHandler( http_posthandler *newhandler )
 *
 * Your newly registered function must do the following
 *
 * 1. Decide what data is needed from the form (most likely by looking at
 * the "url" parameter).
 *
 * 2. Extract this data from the form. In HTML forms all elements have
 * an ASCII name and an ASCII Value. This data is extracted with the
 * function:
 *
 *    int ExtractPostData( PCSTR name,PCSTR PostData, PSTR dest_buffer,int maxlen )
 *
 * For example, if your form has a field with the name "myname", you can
 * get it's value with the call:
 *
 *    int result=ExtractPostData( "myname", pData, where_to_put_the_result, max_result_chars );
 *
 * result = -1 if myname was not found, otherwise it holds the number of
 * chars put in the destination buffer.
 *
 * result = 0 is a valid response, it means that "myname" was present but held no data.
 *
 * If your form has multiple data fields you can make multiple ExtractData() calls.
 * You can also process pData directly if efficiency is of great importance.
 *
 * 3. Do something with the extracted data.
 *
 * 4. Provide an HTML response
 *
 * Thats it for forms...
 *
 *---------------------------------------------------------------------------------*/

#include "formdata.h"

#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>

extern "C"
{
}

extern FormData myData;
extern OS_SEM form_sem;

/* Name: CheckExtractResult
 * Description: This function is used to extract a field from an HTML form sent to the server
 * with a post.
 * Inputs: 
 * char * buffer is the output buffer for the field
 * int buf_size is the size of the output buffer
 * char * pData is the data portion of the post request
 * char * name is the name of the field
 * char * err is an error message to print if something goes wrong
 * Outputs: void
 */
void CheckExtractResult(char* buffer, int buf_size, char *pData, char* name, char* err) {
	int result = ExtractPostData( name, pData, buffer , buf_size);

	if (result == FIELD_NOT_FOUND) {
		iprintf("Field not found. %s", err);
	} else if (result == FIELD_EMPTY) {
		iprintf("No value in field. %s", err);
	}
}


/*-------------------------------------------------------------------
 * When you write a MyDoPost() function and register it, it will
 * be called when a web browser submits a Form POST. This is how
 * the data entered in the web page form gets to the device.
 *-----------------------------------------------------------------*/

/* Name: MyDoPost
 * Description: This the handler for post requests that come into
 * our web server
 * Inputs: int sock represents the file descriptor that we use to
 * communicate with the client
 * char * url is the complete URL of the POST destination
 * char * pData is the data portion of the post request
 * char * rxBuffer is the complete HTTP request
 * Outputs: Always returns 0
 */
int MyDoPost( int sock, char *url, char *pData, char *rxBuffer )
{
	// Insert your post request handling here
	int buf_size = 50;  // arbitrary buffer size
	char buffer[buf_size];
	memset(buffer, 0, buf_size * sizeof(char));


	CheckExtractResult(buffer, buf_size, pData, "maxrpm", "Error on maxrpm");

	myData.SetMaxRPM(buffer);
	memset(buffer, 0, buf_size * sizeof(char));

	CheckExtractResult(buffer, buf_size, pData, "minrpm", "Error on minrpm");
	myData.SetMinRPM(buffer);
	memset(buffer, 0, buf_size * sizeof(char));

	CheckExtractResult(buffer, buf_size, pData, "rotations", "Error on rotations");
	myData.SetRotations(buffer);
	memset(buffer, 0, buf_size * sizeof(char));

	CheckExtractResult(buffer, buf_size, pData, "direction", "Error on direction");
	myData.SetDirection(buffer);
	memset(buffer, 0, buf_size * sizeof(char));

   // We have to respond to the post with a new HTML page...
   // In this case we will redirect so the browser will
   //go to that URL for the response...
   RedirectResponse( sock, "INDEX.HTM" );

   return 0;
}


/* Name: RegisterPost
 * Description: The default Post Request handler is empty. We need to insert
 * our app specific handler in its place.
 * Inputs: None
 * Outputs: None
 */
void RegisterPost()
{
   SetNewPostHandler( MyDoPost );

}
