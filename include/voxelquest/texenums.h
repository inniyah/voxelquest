#ifndef _voxelquest_texenums_h_
#define _voxelquest_texenums_h_

#include "voxelquest/enums.h"

#define E_TEXENTRY(DDD) \
DDD(TEX_NULL) \
DDD(TEX_DEBUG) \
DDD(TEX_UNUSED) \
DDD(TEX_SAND) \
DDD(TEX_STONE) \
DDD(TEX_SNOW) \
DDD(TEX_GRASS) \
DDD(TEX_MORTAR) \
DDD(TEX_WOOD) \
DDD(TEX_BRICK) \
DDD(TEX_SHINGLE) \
DDD(TEX_PLASTER) \
DDD(TEX_EARTH) \
DDD(TEX_BARK) \
DDD(TEX_TREEWOOD) \
DDD(TEX_LEAF) \
DDD(TEX_GOLD) \
DDD(TEX_WATER) \
DDD(TEX_METAL) \
DDD(TEX_GLASS) \
DDD(TEX_MAPLAND) \
DDD(TEX_MAPWATER) \
DDD(TEX_SKY) \
DDD(TEX_SKIN) \
DDD(TEX_LEATHER) \
DDD(TEX_EXPLOSION) \
DDD(TEX_PANTS) \
DDD(TEX_ARMOR) \
DDD(TEX_MEAT) \
DDD(TEX_BONE) \
DDD(TEX_LENGTH)

const std::string E_TEXENTRY_STRINGS[]={
    E_TEXENTRY(DO_DESCRIPTION)
};

enum E_TEXENTRY_VALS
{
    E_TEXENTRY(DO_ENUM)
};


#endif//_voxelquest_texenums_h_