#ifndef _DINLE_COMMANDS_H_
#define _DINLE_COMMANDS_H_

#define DINLE_TAG_START(a, p) "<" a " " p ">"
#define DINLE_TAG_END(a) "</" a ">"
#define DINLE_TAG_ALONE(a, p) "<" a " " p "/>"

#define DINLE_PROTOCOL_VER "001"
#define DINLE_PROTOCOL "DP" DINLE_PROTOCOL_VER

#define DINLE_SERVER_ID "dinle-server"
#define DINLE_CLIENT_ID "dinle-client"

#define DINLE_SERVER_TAG DINLE_TAG_ALONE (DINLE_SERVER_ID, "protocol=\"" DINLE_PROTOCOL "\"")
#define DINLE_CLIENT_TAG DINLE_TAG_ALONE (DINLE_CLIENT_ID, "protocol=\"" DINLE_PROTOCOL "\"")

#define DINLE_COMMAND_END DINLE_TAG_ALONE("end-command","")

#endif
