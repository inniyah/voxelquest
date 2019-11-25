#ifndef _voxelquest_entenums_h_
#define _voxelquest_entenums_h_

// do not rearrange
enum ENT_TYPE
{
    E_ENTTYPE_NPC,
    E_ENTTYPE_WEAPON,
    E_ENTTYPE_OBJ,
    E_ENTTYPE_BULLET,
    E_ENTTYPE_TRACE,
    E_ENTTYPE_DEBRIS,
    E_ENTTYPE_LENGTH
};

enum ENT_SUBTYPE
{

    E_ENTSUB_LENGTH
};


enum E_WEAPON_STATES
{
    E_WEAPON_STATE_IDLE,
    E_WEAPON_STATE_BEG,
    E_WEAPON_STATE_END,
    E_WEAPON_STATE_LENGTH
};

enum E_WEAPON_POS
{
    E_WEAPON_POS_RELAXED,

    E_WEAPON_POS_FORWARD,

    E_WEAPON_POS_LEFT,
    E_WEAPON_POS_RIGHT,
    E_WEAPON_POS_UP,
    E_WEAPON_POS_DOWN,

    E_WEAPON_POS_UP_LEFT,
    E_WEAPON_POS_UP_RIGHT,
    E_WEAPON_POS_DOWN_LEFT,
    E_WEAPON_POS_DOWN_RIGHT,

    E_WEAPON_POS_LENGTH
};


#endif//_voxelquest_entenums_h_