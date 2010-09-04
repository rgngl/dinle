#ifndef _DINLE_COMMANDS_H_
#define _DINLE_COMMANDS_H_

#define DINLE_TAG_START(a, p) "<" a " " p ">"
#define DINLE_TAG_END(a) "</" a ">"
#define DINLE_TAG_ALONE(a, p) "<" a " " p "/>"


#define DINLE_PROTOCOL_VER "001"
#define DINLE_PROTOCOL "DP" DINLE_PROTOCOL_VER


#define DINLE_SERVER_ID "dinle-server"
#define DINLE_CLIENT_ID "dinle-client"


#define DINLE_COMMAND_END "end-command"

#define DINLE_REPLY_CLIENTOK "client-ok"
#define DINLE_REPLY_ERROR "error"


#define DINLE_TAG_SERVER DINLE_TAG_ALONE (DINLE_SERVER_ID, "protocol=\"" DINLE_PROTOCOL "\"")
#define DINLE_TAG_CLIENT DINLE_TAG_ALONE (DINLE_CLIENT_ID, "protocol=\"" DINLE_PROTOCOL "\"")

#define DINLE_TAG_ERROR(msg) DINLE_TAG_START(DINLE_REPLY_ERROR, "") msg DINLE_TAG_END(DINLE_REPLY_ERROR)

#endif
