#ifndef KAYLES_TYPES_H
#define KAYLES_TYPES_H

#include <cstdint>
#include <ctime>

enum Status {
    WAITING_FOR_OPONENT = 0,
    TURN_A = 1,
    TURN_B = 2,
    WIN_A = 3,
    WIN_B = 4
};

enum MessageType {
    MSG_JOIN = 0,
    MSG_MOVE_1 = 1,
    MSG_MOVE_2 = 2,
    MSG_KEEP_ALIVE = 3,
    MSG_GIVE_UP = 4
};

struct ClientMessage {
    MessageType message_type;
    int32_t player_id;
    int32_t game_id;
    int8_t pawn;
};


#endif // KAYLES_TYPES_H
