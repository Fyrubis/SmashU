/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"

#ifndef DEPLOY
#  define ASSETS_PATH "../../assets/"
#else
#  define ASSETS_PATH "./assets/"
#endif


namespace assets
{
    void InitAssets(AssetManager *assets);
    void InitTextures(AssetManager *assets);
    void InitFonts(AssetManager *assets);
    void InitSFX(AssetManager *assets);
    void InitMusic(AssetManager *assets);
    void InitSpriteAnimations(Scene *scene);
    void InitAnimations_FireKnight(AssetManager *assets, SpriteAnimManager *animManager);
    void InitAnimations_WaterPriestress(AssetManager *assets, SpriteAnimManager *animManager);

    std::string AnimIDToString(AnimID animID);
}

namespace game
{
    void UpdateFontSize(Scene *scene);
};

class Colors 
{
public:
    // Basic colors
    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;

    // Game colors
    static const Color Gold;
    static const Color DarkBlue;
};

enum SoundID : uint32_t
{
    SFX_SWORD_ATTACK_1, SFX_SWORD_ATTACK_2, SFX_SWORD_ATTACK_3,
    SFX_SWORD_HIT_A1, SFX_SWORD_HIT_A2, SFX_SWORD_HIT_A3,
    SFX_SWORD_HIT_B1, SFX_SWORD_HIT_B2, SFX_SWORD_HIT_B3,
    SFX_FIRE_1, SFX_FIRE_2,
    SFX_WATER, SFX_ICE, SFX_EXPLOSION,

    SFX_WHOOSH_1, SFX_WHOOSH_2,
    SFX_JUMP_AIR, SFX_LAND, SFX_ROLL, SFX_JUMP_GROUND,

    SFX_UI_SELECTED, SFX_UI_CLICK, SFX_UI_ITEM, SFX_UI_BACK, SFX_UI_VALIDATE,
    SFX_UI_PAUSE, SFX_UI_UNPAUSE, SFX_UI_COUNTDOWN
};

enum MusicID : uint32_t
{
    MUSIC_MENU, MUSIC_STAGE
};

enum TextureID : uint32_t
{
    TEXTURE_STAR_FIELDS_0, TEXTURE_STAR_FIELDS_1, TEXTURE_STAR_FIELDS_2
};

enum SheetID : uint32_t
{
    SHEET_FIRE_KNIGHT_0, SHEET_FIRE_KNIGHT_1,
    SHEET_WATER_PRIESTESS_0, SHEET_WATER_PRIESTESS_1,
    SHEET_SHIELD,

    SHEET_TILESET_STAR_FIELDS,
    SHEET_UI, SHEET_UI_STAGE,

    SHEET_ITEM_POTION, SHEET_ITEM_BOMB,

    SHEET_VFX_DASH_SMOKE_1, SHEET_VFX_DASH_SMOKE_2,
    SHEET_VFX_IMPACT_DUST, SHEET_VFX_SIDE_IMPACT, SHEET_VFX_BIG_DUST,
    SHEET_VFX_DUST_1, SHEET_VFX_DUST_2, SHEET_VFX_DUST_3, SHEET_VFX_DUST_4,

    SHEET_VFX_HIT, SHEET_VFX_PARTICLES,
    SHEET_VFX_POWER, SHEET_VFX_POTION, SHEET_VFX_HEAL, SHEET_VFX_EXPLOSION,
};

enum FontID : uint32_t
{
    FONT_SMALL,
    FONT_NORMAL,
    FONT_LARGE,

    FONT_DAMAGE = FONT_LARGE,
    FONT_TEXT,
    FONT_TIME,

    //
    FONT_COUNT
};

enum class AnimCategory : uint32_t
{
    DEFAULT,
    FIRE_KNIGHT_0, FIRE_KNIGHT_1,
    WATER_PRIESTESS_0, WATER_PRIESTESS_1,
    ITEM,
    VFX,
    DUST
};

enum class AnimType : uint32_t
{
    UNDEFINED,
    IDLE, RUN, SKID, ROLL,
    JUMP_UP, JUMP_TOP, JUMP_DOWN,
    ATTACK_1, ATTACK_2, ATTACK_3, ATTACK_1_END, ATTACK_2_END, ATTACK_AIR,
    SMASH_START, SMASH_HOLD, SMASH_RELEASE,
    DEFEND_START, DEFEND, DEFEND_END, TAKE_HIT,
    PLAYER_INTRO,
    //
    SHIELD,
    //
    POTION, BOMB_NORMAL, BOMB_DANGER,
    //
    SMALL_DUST_1, SMALL_DUST_2, ROLL_DUST, SKID_DUST, LAND_DUST, JUMP_DUST, SIDE_IMPACT,
    //
    SQUARE_PARTICULE, CHARGED_PARTICULE, POWER_AURA,
    HEAL, EXPLOSION,
};

constexpr uint32_t AnimID_Make(AnimCategory category, AnimType type)
{
    return (static_cast<uint32_t>(category) << 16) | static_cast<uint32_t>(type);
}
constexpr AnimType AnimID_GetType(uint32_t animID)
{
    return static_cast<AnimType>(animID & static_cast<uint32_t>((1 << 16) - 1));
}
constexpr AnimCategory AnimID_GetCategory(uint32_t animID)
{
    return static_cast<AnimCategory>(animID >> 16);
}
