/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/water_priestess_system.h"
#include "ecs/player/player_controller_system.h"
#include "ecs/player/player_utils.h"

void WaterPriestessSystem::OnFixedUpdate(EntityCommandBuffer &ecb)
{
    auto viewAnim = m_registry.view<
        const WaterPriestessTag,
        const PlayerAffiliation,
        PlayerController,
        SpriteAnimState>();
    for (auto [entity, affiliation, controller, anim] : viewAnim.each())
    {
        if (controller.isStateUpdated == false) continue;

        // TODO S'inspirer du Fire Knight
    }
}

void WaterPriestessSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto viewAnim = m_registry.view<
        const WaterPriestessTag,
        const PlayerAffiliation,
        const PlayerControllerInput,
        const Transform,
        SpriteAnimState,
        const PlayerController,
        PlayerAnimInfo
    >();
    for (auto [entity, affiliation, input, transform, spriteAnim, controller, animInfo] : viewAnim.each())
    {
        assert(spriteAnim.saveEvents);
        for (int i = 0; i < spriteAnim.eventCount; i++)
        {
            SpriteAnimEvent &animEvent = spriteAnim.events[i];

            if (animEvent.type == SpriteAnimEvent::Type::CYCLE_END)
            {
                OnAnimCycleEnd(entity, animEvent, spriteAnim, affiliation, input, animInfo);
            }
            else if (animEvent.type == SpriteAnimEvent::Type::FRAME_CHANGED)
            {
                OnAnimFrameChanged(entity, animEvent, transform, affiliation, controller, animInfo);
            }
        }
    }
}

void WaterPriestessSystem::OnAnimFrameChanged(
    entt::entity entity,
    const SpriteAnimEvent &animEvent,
    const Transform &transform,
    const PlayerAffiliation &affiliation,
    const PlayerController &controller,
    PlayerAnimInfo &animInfo)
{
    // TODO S'inspirer du Fire Knight
}

void WaterPriestessSystem::OnAnimCycleEnd(
    entt::entity entity,
    const SpriteAnimEvent &animEvent,
    SpriteAnimState &anim,
    const PlayerAffiliation &affiliation,
    const PlayerControllerInput &input,
    PlayerAnimInfo &event)
{
    // TODO S'inspirer du Fire Knight
}
