/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "scene_manager/stage_manager.h"
#include "imgui/imgui_manager.h"

#include "ecs/common/camera.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/kinematic_utils.h"
#include "ecs/common/terrain_utils.h"
#include "ecs/common/tracked_target.h"
#include "ecs/common/visual_id.h"

#include "ecs/player/player_components.h"
#include "ecs/player/player_command.h"
#include "ecs/player/player_input_system.h"
#include "ecs/player/player_controller_system.h"
#include "ecs/player/shield.h"
#include "ecs/player/fire_knight_system.h"
#include "ecs/player/water_priestess_system.h"
#include "ecs/player/player_ai.h"

#include "ecs/item/potion.h"
#include "ecs/item/bomb.h"

#include "ecs/stage/star_fields_terrain.h"
#include "ecs/stage/star_fields_platform.h"

StageManager::StageManager(InputManager *inputManager)
    : SceneManager(inputManager)
    , m_oneWayCallback(GetScene())
    , m_state(State::FIGHT)
    , m_paused(false)
    , m_pauseMenu(nullptr)
    , m_delayStage(0.f)
    , m_delayBomb(-1.f)
    , m_delayPotion(-1.f)
{
    Scene *scene = GetScene();
    entt::registry &registry = scene->GetRegistry();
    SpriteAnimManager *animManager = scene->GetAnimManager();
    animManager->SetAnimIDToString(assets::AnimIDToString);

    AssetManager *assets = scene->GetAssetManager();
    assets::InitAssets(assets);
    assets::InitTextures(assets);
    assets::InitFonts(assets);
    assets::InitMusic(assets);
    assets::InitSFX(assets);
    assets::InitSpriteAnimations(scene);

    std::shared_ptr<ImGuiManager> imGuiManager = std::make_shared<ImGuiManager>(scene);
    scene->SetImGuiManager(imGuiManager);
    imGuiManager->SetFlags(
        IM_GUI_PHYSICS |
        IM_GUI_QUERY |
        IM_GUI_ENTITY_INSPECTOR
    );

    //--------------------------------------------------------------------------
    // Simulation Systems

    // Transform & terrain
    scene->AddSimulationSystem(std::make_shared<TransformSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<KinematicTargetSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<AnimatorSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<OneWaySystem>(scene));
    scene->AddSimulationSystem(std::make_shared<FloatingPlatformSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<GroundSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<TrackedTargetSystem>(scene));

    // Player
    scene->AddSimulationSystem(std::make_shared<PlayerInputSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<PlayerAISystem>(scene));
    scene->AddSimulationSystem(std::make_shared<PlayerControllerSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<FireKnightSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<WaterPriestessSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<PlayerShieldSystem>(scene));

    // Item
    scene->AddSimulationSystem(std::make_shared<BombSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<PotionSystem>(scene));

    // Other
    scene->AddSimulationSystem(std::make_shared<VisualIDSimulationSystem>(scene));
    scene->AddSimulationSystem(std::make_shared<WorldBoundsSystem>(scene));

    //--------------------------------------------------------------------------
    // Presentation Systems

    scene->AddPresentationSystem(std::make_shared<CameraSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderGameSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderUISystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderQuerySystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderPhysicsSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderGridSystem>(scene));
    scene->AddPresentationSystem(std::make_shared<RenderImGuiSystem>(scene, imGuiManager));

    //--------------------------------------------------------------------------
    // Composants ImGui

    // Transform & physics
    imGuiManager->AddComponent(std::make_unique<ImGuiTransform>());
    imGuiManager->AddComponent(std::make_unique<ImGuiFixedUpdateTransform>());
    imGuiManager->AddComponent(std::make_unique<ImGuiLocalTransform>());
    imGuiManager->AddComponent(std::make_unique<ImGuiRigidbody>());
    imGuiManager->AddComponent(std::make_unique<ImGuiReferencePosition>());
    imGuiManager->AddComponent(std::make_unique<ImGuiKinematicTargetPosition>());
    imGuiManager->AddComponent(std::make_unique<ImGuiKinematicTargetRotation>());

    // Render
    imGuiManager->AddComponent(std::make_unique<ImGuiRenderSortingLayer>());
    imGuiManager->AddComponent(std::make_unique<ImGuiRenderBlendMod>());
    imGuiManager->AddComponent(std::make_unique<ImGuiRenderColorMod>());
    imGuiManager->AddComponent(std::make_unique<ImGuiSprite>());
    imGuiManager->AddComponent(std::make_unique<ImGuiTiledSprite>());
    imGuiManager->AddComponent(std::make_unique<ImGuiTilemapRenderer>());
    imGuiManager->AddComponent(std::make_unique<ImGuiBackgroundLayer>());
    imGuiManager->AddComponent(std::make_unique<ImGuiSpriteAnimState>(scene));

    // Camera
    imGuiManager->AddComponent(std::make_unique<ImGuiCamera>());
    imGuiManager->AddComponent(std::make_unique<ImGuiCameraFollow>());

    // Ground
    imGuiManager->AddComponent(std::make_unique<ImGuiGroundContact>());
    imGuiManager->AddComponent(std::make_unique<ImGuiOneWayPass>());

    // Player
    imGuiManager->AddComponent(std::make_unique<ImGuiDamageable>());
    imGuiManager->AddComponent(std::make_unique<ImGuiPlayerAffiliation>());
    imGuiManager->AddComponent(std::make_unique<ImGuiPlayerAI>());
    imGuiManager->AddComponent(std::make_unique<ImGuiPlayerControllerInput>());
    imGuiManager->AddComponent(std::make_unique<ImGuiPlayerController>());
    imGuiManager->AddComponent(std::make_unique<ImGuiPlayerAnimInfo>());
    imGuiManager->AddComponent(std::make_unique<ImGuiTrackedTarget>());

    // Other
    imGuiManager->AddComponent(std::make_unique<ImGuiVisualID>());

    // Tag
    imGuiManager->AddTag(std::make_unique<ImGuiTag<ShieldTag>>("Shield tag"));
    imGuiManager->AddTag(std::make_unique<ImGuiTag<FireKnightTag>>("Fire knight tag"));
    imGuiManager->AddTag(std::make_unique<ImGuiTag<WaterPriestessTag>>("Water priestess tag"));
    imGuiManager->AddTag(std::make_unique<ImGuiTag<PotionTag>>("Potion tag"));
    imGuiManager->AddTag(std::make_unique<ImGuiTag<BombTag>>("Bomb tag"));

    //--------------------------------------------------------------------------
    // Cr�ation du niveau

    // Stats
    g_gameCommon.ResetPlayerStats();

    // Stage
    InitStarFields();

    // Limite de temps
    if (g_gameCommon.stageConfig.mode == StageConfig::Mode::LIMITED_TIME)
    {
        m_delayStage = (float)g_gameCommon.stageConfig.duration * 60.f;
    }

    // Cam�ra
    CameraCommand::Create(registry, registry.create(), scene, b2Vec2{ 0.f, 1.f });

    // Cr�e l'interface utilisateur
    m_stageHUD = new UIStageHUD(scene);

    // D�lais pour les potions et les bombes
    m_delayBomb = random::RangeF(5.f, 15.f);
    m_delayPotion = random::RangeF(5.f, 15.f);

    // BONUS - Modifiez les d�lais en fonctions des param�tres du jeu
    
    //const StageConfig &stageConfig = g_gameCommon.stageConfig;
    //switch (stageConfig.bombsFrequency)
    //{
    //    case StageConfig::Frequency::RARELY:
    //    case StageConfig::Frequency::SOMETIMES:
    //    case StageConfig::Frequency::OFTEN:
    //        break;
    //    default:
    //        break;
    //}
}

StageManager::~StageManager()
{
}

void StageManager::OnSceneUpdate()
{
    Scene *scene = GetScene();
    InputManager *inputManager = scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);

    m_delayStage -= scene->GetDelta();
    PlayerStats *player;
    switch(g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:
        for (int i = 0; i < g_gameCommon.playerCount; i++) {
            player = g_gameCommon.GetPlayerStats(i);
            if (player->fallCount >= g_gameCommon.stageConfig.lifeCount)
                QuitScene();
        }
        break;
    case StageConfig::Mode::LIMITED_TIME:
        if (m_delayStage < 0.f)
        {
            QuitScene();
        }
        break;
    }

    if (applicationInput->pausePressed)
    {
        if (m_paused)
        {
            assert(m_pauseMenu != nullptr);

            m_pauseMenu->PlayFadeOut();
            scene->GetAssetManager()->PlaySoundFX(SFX_UI_UNPAUSE);
        }
        else
        {
            assert(m_pauseMenu == nullptr);

            m_pauseMenu = new UIPauseMenu(scene);
            m_pauseMenu->SetParent(scene->GetCanvas());
            m_pauseMenu->PlayFadeIn();

            scene->GetTime().SetTimeScale(0.0f);
            m_paused = true;

            scene->GetAssetManager()->PlaySoundFX(SFX_UI_PAUSE);
        }
    }

    game::UpdateFontSize(scene);
}

void StageManager::OnSceneFixedUpdate()
{
    Scene *scene = GetScene();
    const StageConfig &stageConfig = g_gameCommon.stageConfig;
    if (m_state == State::FIGHT)
    {
        if (stageConfig.bombsFrequency != StageConfig::Frequency::NEVER)
        {
            m_delayBomb -= scene->GetDelta();
            if (m_delayBomb < 0.f)
            {
                AddBomb();
            }
        }
        if (stageConfig.potionFrequency != StageConfig::Frequency::NEVER)
        {
            m_delayPotion -= scene->GetDelta();
            if (m_delayPotion < 0.f)
            {
                AddPotion();
            }
        }
    }
}

void StageManager::QuitPause()
{
    m_pauseMenu->Delete();
    m_pauseMenu = nullptr;
    m_paused = false;
    GetScene()->GetTime().SetTimeScale(1.f);
}

StageManager *StageManager::GetFromScene(Scene *scene)
{
    if (scene == nullptr)
    {
        assert(false);
        return nullptr;
    }
    return dynamic_cast<StageManager *>(scene->GetSceneManager());
}

void StageManager::AddPotion()
{
    Scene *scene = GetScene();
    entt::registry &registry = scene->GetRegistry();

    b2Vec2 position = { random::RangeF(-6.f, 6.f), 10.f };
    PotionCommand::Create(registry, registry.create(), scene, position);

    m_delayPotion = random::RangeF(15.f, 30.f);
}

void StageManager::AddBomb()
{
    Scene *scene = GetScene();
    entt::registry &registry = scene->GetRegistry();

    b2Vec2 position = { random::RangeF(-7.f, 7.f), 10.f };
    b2Vec2 velocity = { random::RangeF(-3.f, 3.f), 0.f };
    BombCommand::Create(registry, registry.create(), scene, position, velocity);

    m_delayBomb = random::RangeF(7.f, 20.f);
}

void StageManager::InitStarFields()
{
    Scene *scene = GetScene();
    AssetManager *assets = scene->GetAssetManager();
    entt::registry &registry = scene->GetRegistry();

    // Stage
    StarFieldsTerrainCommand::Create(registry, registry.create(), scene, b2Vec2_zero);
    StarFieldsPlatformCommand::Create(registry, registry.create(), scene, b2Vec2{ -7.0f, 4.5f }, 0);

    // Cr�e les joueurs
    b2Vec2 startPositions[4] = {
        b2Vec2(-6.f, 0.05f), b2Vec2(+6.f, 0.05f),
        b2Vec2(-2.f, 0.05f), b2Vec2(+2.f, 0.05f),
    };
    for (int playerID = 0; playerID < g_gameCommon.playerCount; playerID++)
    {
        PlayerCommand::Create(
            registry, registry.create(), scene,
            playerID, startPositions[playerID % 4]
        );
    }

    // Background
    float factors[] = { 0.05f, 0.3f, 0.6f };
    LayerMode modes[] = {
        LayerMode::FILL_VERTICAL,
        LayerMode::FILL_BELOW,
        LayerMode::FILL_BELOW
    };
    TextureID textureIDs[] = {
        TEXTURE_STAR_FIELDS_0,
        TEXTURE_STAR_FIELDS_1,
        TEXTURE_STAR_FIELDS_2
    };
    for (int i = 0; i < 3; i++)
    {
        entt::entity layerEntity = registry.create();
        RenderSortingLayer renderLayer{};
        renderLayer.enabled = true;
        renderLayer.layer = LAYER_BACKGROUND;
        renderLayer.orderInLayer = i;

        BackgroundLayer backgroundLayer(assets->GetTexture(textureIDs[i]));
        backgroundLayer.SetDimensions(14.f);
        backgroundLayer.mode = modes[i];
        backgroundLayer.shiftFactor = b2Vec2{ factors[i], 0.9f * factors[i] };

        b2Vec2 position = b2Vec2{ 0.f, -5.f } + 0.5f * backgroundLayer.dimensions;

        registry.emplace<NameComponent>(layerEntity, "Background layer " + std::to_string(i));
        registry.emplace<BackgroundLayer>(layerEntity, backgroundLayer);
        registry.emplace<Transform>(layerEntity, position);
        registry.emplace<RenderSortingLayer>(layerEntity, renderLayer);
    }

    // Music
    scene->GetAssetManager()->FadeInMusic(MUSIC_STAGE);
}
