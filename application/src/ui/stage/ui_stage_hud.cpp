/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/stage/ui_stage_hud.h"
#include "scene_manager/stage_manager.h"

UIStageHUD::UIStageHUD(Scene* scene)
    : UIObject(scene)
    , m_damageTexts()
    , m_timeText(nullptr)
{
    SetName("Stage HUD");
    SetFadeChildren(false);

    AssetManager* assets = scene->GetAssetManager();
    SpriteSheet* spriteSheet = nullptr;
    SpriteGroup* spriteGroup = nullptr;
    TTF_Font* font = nullptr;

    int playerCount = g_gameCommon.playerCount;

    //--------------------------------------------------------------------------
    // Grid layout
    UIGridLayout* hLayout;
    switch (g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:
        hLayout = new UIGridLayout(m_scene, 1, playerCount * 2);
        hLayout->SetParent(this);
        hLayout->SetAnchor(Anchor::SOUTH);
        hLayout->SetPadding(10.f);
        hLayout->SetColumnSpacing(12.5f);
        hLayout->SetRowSize(30.f);
        for (int i = 0; i < playerCount; i++) {
            hLayout->SetColumnSize(i*2, 70.f);  // Percent
            hLayout->SetColumnSize(i*2 +1, 30.f); // Lives
            if(i*2 -1 > 0)
                hLayout->SetColumnSpacing(i*2 -1, 60.f);
        }
        break;
    case StageConfig::Mode::LIMITED_TIME:
        hLayout = new UIGridLayout(m_scene, 1, playerCount);
        hLayout->SetParent(this);
        hLayout->SetAnchor(Anchor::SOUTH);
        hLayout->SetPadding(10.f);
        hLayout->SetColumnSpacing(50.f);
        hLayout->SetRowSize(30.f);
        hLayout->SetColumnSize(80.f);
        break;
    }

    //--------------------------------------------------------------------------
    // Informations sur les joueurs


    spriteSheet = assets->GetSpriteSheet(SHEET_UI);
    AssertNew(spriteSheet);
    spriteGroup = spriteSheet->GetGroup("PlayerBorder");
    AssertNew(spriteGroup);

    UIImage* fillImage;
    switch (g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:

        for (int j = 0; j < playerCount * 2; j++) {
            fillImage = new UIImage(m_scene, spriteGroup, 1);
            fillImage->SetBorders(6, 6, 6, 6, 4.f);
            fillImage->SetOpacity(0.5f);
            fillImage->SetLayer(LAYER_UI_BACKGROUND);
            fillImage->SetParent(this);
            hLayout->AddObject(fillImage, 0, j);
        }

        for (int i = 0; i < playerCount; i++)
        {
            // Compteur des dégats
            font = assets->GetFont(FONT_DAMAGE);
            UIText* text = new UIText(scene, "0%", font, Colors::White);
            text->SetAnchor(Anchor::CENTER);

            hLayout->AddObject(text, 0, i * 2);

            m_damageTexts.push_back(text);
        }
        break;

    case StageConfig::Mode::LIMITED_TIME: break;
    }

    // Compteur du temps restant
    switch (g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:
        break;
    case StageConfig::Mode::LIMITED_TIME:
        font = assets->GetFont(FONT_TIME);
        m_timeText = new UIText(scene, "0:00", font, Colors::White);
        m_timeText->SetParent(this);
        m_timeText->SetAnchor(Anchor::NORTH_EAST);
        m_timeText->GetLocalRect().offsetMax = { -10.f, -10.f };
        break;
    }
}

void UIStageHUD::Update()
{
    UIObject::Update();

    StageManager* stageManager = StageManager::GetFromScene(m_scene);
    const int playerCount = g_gameCommon.playerCount;

    switch (g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:
        break;
    case StageConfig::Mode::LIMITED_TIME:
        int minutes = (int)stageManager->GetRemainingTime() / 60;
        int seconds = (int)stageManager->GetRemainingTime() % 60;
        char buffer[128] = { 0 };

        sprintf_s(buffer, "%d:%02d", minutes, seconds);
        m_timeText->SetString(buffer);
        break;
    }

    for (int i = 0; i < playerCount; i++)
    {
        const int score = static_cast<int>(g_gameCommon.playerStats[i].ejectionScore);
        m_damageTexts[i]->SetString(std::to_string(score) + "%");
    }
}
