/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/Metal_Bladekeeper_system.h"
#include "ecs/player/player_controller_system.h"
#include "ecs/player/player_utils.h"

void MetalBladekeeperSystem::OnFixedUpdate(EntityCommandBuffer& ecb)
{
    auto viewAnim = m_registry.view<
        const MetalBladekeeperTag,
        const PlayerAffiliation,
        PlayerController,
        SpriteAnimState>();
    for (auto [entity, affiliation, controller, anim] : viewAnim.each())
    {
        if (controller.isStateUpdated == false) continue;

        PlayerConfig* config = g_gameCommon.GetPlayerConfig(affiliation.playerID);
        if (config == nullptr) continue;

        // TODO S'inspirer du Fire Knight
        AnimCategory animCat = config->category;
        AnimType type = AnimType::UNDEFINED;
        switch (controller.currState)
        {
        case PlayerState::IDLE: type = AnimType::IDLE; break;
        case PlayerState::RUN: type = AnimType::RUN; break;
        case PlayerState::JUMP: type = AnimType::JUMP_UP; break;
        case PlayerState::FALL: type = AnimType::JUMP_TOP; break;
        case PlayerState::DEFEND: type = AnimType::DEFEND_START; break;
        case PlayerState::LAUNCHED: type = AnimType::JUMP_UP; break;
        case PlayerState::TAKE_DAMAGE: type = AnimType::TAKE_HIT; break;
        case PlayerState::ROLL: type = AnimType::ROLL;break;
        //
        case PlayerState::ATTACK_COMBO: type = AnimType::ATTACK_1; break;
        case PlayerState::ATTACK_SPECIAL: type = AnimType::ATTACK_2; break;
        case PlayerState::SMASH_HOLD: type = AnimType::SMASH_START; break;
        case PlayerState::SMASH_RELEASE: type = AnimType::SMASH_RELEASE; break;
        case PlayerState::ATTACK_AIR: type = AnimType::ATTACK_AIR; break;
        default: break;
        }

        if (type != AnimType::UNDEFINED)
        {
            SpriteAnimUtils::SetAnimation(anim, AnimID_Make(config->category, type));
        }
    }
}

void MetalBladekeeperSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto viewAnim = m_registry.view<
        const MetalBladekeeperTag,
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

void MetalBladekeeperSystem::OnAnimFrameChanged(
    entt::entity entity,
    const SpriteAnimEvent &animEvent,
    const Transform &transform,
    const PlayerAffiliation &affiliation,
    const PlayerController &controller,
    PlayerAnimInfo &animInfo)
{
    // TODO S'inspirer du Fire Knight
    const int playerID = affiliation.playerID;
    PlayerConfig* config = g_gameCommon.GetPlayerConfig(playerID);
    if (config == nullptr) return;

    QueryFilter filter;
    filter.categoryBits = CATEGORY_ATTACK;
    filter.maskBits = config->otherTeamMask | CATEGORY_ITEM;
    filter.solidOnly = true;

    const float lockTime = 0.2f;
    float s = controller.facingRight ? 1.f : -1.f;
    AnimType animType = AnimID_GetType(animEvent.id);

    if (animType == AnimType::ATTACK_1)
    {
        animInfo.autoVelocity = 0.0f;

        if (animEvent.index == 1)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_SWORD_ATTACK_3, SFXIntensity::NORMAL);
        }
        if (animEvent.index == 1)
        {
            // TODO - Effectuez l'attaque sur la frame d'indice 4 et non celle d'indice 2.

            b2Vec2 position = transform.position;

            Damage damage;
            damage.attackCenter = position;
            damage.amount = 3.f;
            damage.ejectionType = Damage::Type::NO_EJECTION;
            damage.lockTime = lockTime;
            damage.lockAttackTime = 10.5f * PLAYER_ATTACK_FRAME_TIME;

            // TODO - Modifiez le rayon du cercle d'attaque avec la valeur trouvée
            //        en utilisant les outils de debug dans le jeu.

            const b2Vec2 center = transform.position + b2Vec2{ s * 0.8f, 1.5f };
            const float radius = 1.3f;
            bool hit = DamageUtils::AttackCircle(m_scene, entity, affiliation, damage, filter, center, radius);
            PlayerUtils::PlaySFXHit(m_scene, playerID, SFX_SWORD_HIT_A1, SFXIntensity::NORMAL, hit);
        }
    }
    else if (animType == AnimType::ATTACK_2)
    {
        // 
        animInfo.autoVelocity = 0.0f;

        if (animEvent.index == 0)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_SWORD_ATTACK_3, SFXIntensity::NORMAL);
        }
        if (animEvent.index == 0)
        {

            b2Vec2 position = transform.position;

            Damage damage;
            damage.attackCenter = position;
            damage.amount = 3.f;
            damage.ejectionType = Damage::Type::NO_EJECTION;
            damage.lockTime = lockTime;
            damage.lockAttackTime = 10.5f * PLAYER_ATTACK_FRAME_TIME;

            //

            const b2Vec2 center = transform.position + b2Vec2{ s * 0.8f, 1.5f };
            const float radius = 1.3f;
            bool hit = DamageUtils::AttackCircle(m_scene, entity, affiliation, damage, filter, center, radius);
            PlayerUtils::PlaySFXHit(m_scene, playerID, SFX_SWORD_HIT_A1, SFXIntensity::NORMAL, hit);
        }
       
    }
    else if (animType == AnimType::ATTACK_3)
    {
        animInfo.autoVelocity = 0.f;

        if (animEvent.index == 2)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_FIRE_2, SFXIntensity::STRONG);
        }
        else if (animEvent.index == 3)
        {
            // TODO - Modifiez les dommages pour effectuer une ejection.

            b2Vec2 position = transform.position;
            Damage damage;
            damage.attackCenter = position;
            damage.amount = 1.f;
            damage.ejectionType = Damage::Type::FIXED_DIRECTION;
            damage.direction = math::UnitVectorDeg(90.f - s * 45.f);
            damage.ejectionSpeed = 9.f;
            damage.lockTime = lockTime;
            damage.lockAttackTime = 10.5f * PLAYER_ATTACK_FRAME_TIME;

            const b2Vec2 boxCenter = transform.position + b2Vec2{ s * 2.f, 1.5f };
            bool hit = DamageUtils::AttackBox(m_scene, entity, affiliation, damage, filter, boxCenter, 2.f, 1.5f);
            PlayerUtils::PlaySFXHit(m_scene, playerID, SFX_SWORD_HIT_A1, SFXIntensity::STRONG, hit);
        }
    }
    else if (animType == AnimType::ATTACK_AIR)
    {
        animInfo.autoVelocity = 0.f;

        if (animEvent.index == 2)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_FIRE_2, SFXIntensity::STRONG);
        }
        else if (animEvent.index == 3)
        {
            // TODO - Modifiez les dommages pour effectuer une ejection.

            b2Vec2 position = transform.position;
            Damage damage;
            damage.attackCenter = position;
            damage.amount = 1.f;
            damage.ejectionType = Damage::Type::NO_EJECTION;
            damage.direction = math::UnitVectorDeg(90.f - s * 45.f);
            damage.ejectionSpeed = 9.f;
            damage.lockTime = lockTime;
            damage.lockAttackTime = 10.5f * PLAYER_ATTACK_FRAME_TIME;

            const b2Vec2 boxCenter = transform.position + b2Vec2{ s * 1.f, 1.5f };
            bool hit = DamageUtils::AttackBox(m_scene, entity, affiliation, damage, filter, boxCenter, 2.f, 1.5f);
            PlayerUtils::PlaySFXHit(m_scene, playerID, SFX_SWORD_HIT_A1, SFXIntensity::STRONG, hit);
        }
    }
    else if (animType == AnimType::SMASH_RELEASE)
    {      

        if (animEvent.index == 2)
        {
            PlayerUtils::PlaySFXAttack(m_scene, playerID, SFX_FIRE_2, SFXIntensity::MAX);
        }
        else if (animEvent.index == 5)
        {
            Damage damage;
            damage.attackCenter = transform.position + b2Vec2{ s * -2.f , -1.f };
            damage.amount = controller.smashMultiplier * 10.f;
            damage.ejectionType = Damage::Type::CENTER;
            damage.ejectionSpeed = controller.smashMultiplier * 12.f;
            damage.lockTime = lockTime;
            damage.lockAttackTime = 5.5f * PLAYER_ATTACK_FRAME_TIME;

            PlayerUtils::EmitLandDust(m_scene, transform.position, controller.facingRight);

            const b2Vec2 center = transform.position + b2Vec2{ s * 0.1f, 1.f };
            bool hit = DamageUtils::AttackCircle(m_scene, entity, affiliation, damage, filter, center, 2.5f);
            PlayerUtils::PlaySFXHit(m_scene, playerID, SFX_SWORD_HIT_A2, SFXIntensity::MAX, hit);
        }
    }
    else if (animType == AnimType::RUN)
    {
        switch (animEvent.index)
        {
        case 1:
            PlayerUtils::EmitSmallDust(m_scene, transform.position, controller.facingRight, s * 0.6f);
            break;
        case 5:
            PlayerUtils::EmitSmallDust(m_scene, transform.position, controller.facingRight, s * 0.1f);
            break;
        default: break;
        }
    }
}

void MetalBladekeeperSystem::OnAnimCycleEnd(
    entt::entity entity,
    const SpriteAnimEvent &animEvent,
    SpriteAnimState &anim,
    const PlayerAffiliation &affiliation,
    const PlayerControllerInput &input,
    PlayerAnimInfo &event)
{
    // TODO S'inspirer du Fire Knight
    PlayerConfig* config = g_gameCommon.GetPlayerConfig(affiliation.playerID);
    if (config == nullptr) return;

    AnimType nextAnimType = AnimType::UNDEFINED;
    switch (AnimID_GetType(animEvent.id))
    {
    case AnimType::DEFEND_START:
    {
        nextAnimType = AnimType::DEFEND;
        break;
    }
    case AnimType::JUMP_TOP:
    {
        nextAnimType = AnimType::JUMP_DOWN;
        break;
    }
    case AnimType::ATTACK_1:
    {
        // TODO - Décommenter l'enchaînement d'animation après la première attaque.

        if (input.attackDown)
        {
            nextAnimType = AnimType::ATTACK_2;
        }
        else
        {
            nextAnimType = AnimType::ATTACK_1_END;
        }
        break;
    }
    case AnimType::ATTACK_2:
    {
        if (input.attackDown)
        {
            nextAnimType = AnimType::ATTACK_3;
        }
        else
        {
            nextAnimType = AnimType::ATTACK_2_END;
        }
        break;
    }
    case AnimType::ATTACK_1_END:
    case AnimType::ATTACK_2_END:
    case AnimType::ATTACK_3:
    {
        event.type = PlayerAnimInfo::Event::COMBO_END;
        break;
    }
    case AnimType::TAKE_HIT:
    {
        event.type = PlayerAnimInfo::Event::TAKE_HIT_END;
        break;
    }
    //
    case AnimType::ATTACK_AIR:
    {
        nextAnimType = AnimType::ATTACK_1_END;
        break;
    }
    // TODO - Décommentez le code suivant.

    case AnimType::SMASH_START:
    {
        nextAnimType = AnimType::SMASH_HOLD;
        break;
    }
    case AnimType::SMASH_RELEASE:
    {
        event.type = PlayerAnimInfo::Event::SMASH_END;
        break;
    }
    default: break;
    }

    if (nextAnimType != AnimType::UNDEFINED)
    {
        SpriteAnimUtils::SetAnimation(anim, AnimID_Make(config->category, nextAnimType));
    }
}
