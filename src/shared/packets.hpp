#pragma once

enum PacketTypes {
    PLAYER_CONNECT = 0,
    PLAYER_DISCONNECT,
    UPDATE_POSITION,
    SEND_OWN_ID,
    SKIN_LINK,
    SET_USERNAME,
};