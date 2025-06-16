/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"

class PlayerInputSystem : public System
{
public:
    PlayerInputSystem(Scene *scene) : System(scene, "Player input system") {}

    virtual void OnUpdate(EntityCommandBuffer &ecb) override;
};
