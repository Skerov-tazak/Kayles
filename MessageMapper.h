#ifndef MESSAGEMAPPER_H
#define MESSAGEMAPPER_H

#include <cstdint>
#include <ctime>
#include <netinet/in.h>
#include<cstring>

#include "kayles_types.h"
#include "GameState.h"

ClientMessage mapper_parse_client_message(char*);

void mapper_encode_client_message(ClientMessage*, char*);

GameState mapper_parse_gamestate_message(char*);

void mapper_encode_gamestate_message(GameState*, char*);

#endif // MESSAGEMAPPER_H
