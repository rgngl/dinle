#ifndef _DINLE_COMMANDS_H_
#define _DINLE_COMMANDS_H_

#define XML_TAG_START(a, p) "<" a " " p ">"
#define XML_TAG_END(a) "</" a ">"
#define XML_TAG_ALONE(a) "<" a "/>"

#define DINLE_COMMAND_END XML_TAG_ALONE("end-command")

#endif
