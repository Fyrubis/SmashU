/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/title/ui_stage_page.h"
#include "ui/custom_default/ui_default_cursor.h"
#include "scene_manager/title_manager.h"

UIStagePage::UIStagePage(Scene *scene)
    : UIObject(scene)
    , m_ratioHeader(0.2f)
    , m_header(nullptr)
    , m_content(nullptr)
    , m_group(nullptr)
{
    SetName("Stage page");
    SetParent(m_scene->GetCanvas());

    InputManager *inputManager = m_scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);

    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = nullptr;
    SpriteGroup *spriteGroup = nullptr;
    TTF_Font *font = nullptr;

    m_group = new UISelectableGroup(m_scene);
    m_group->SetParent(this);
    m_group->AddInput(&(applicationInput->uiInputs[0]));
    m_group->SetInteractable(true);

    //--------------------------------------------------------------------------
    // Header

    m_header = new UIObject(scene);
    m_header->SetName("UI Stage Header panel");
    m_header->SetParent(this);
    m_header->GetLocalRect().anchorMin = { 0.f, 1.f - m_ratioHeader };
    m_header->GetLocalRect().anchorMax = { 1.f, 1.f };

    UIFillRect *headerFill = new UIFillRect(scene, Colors::DarkBlue);
    headerFill->SetParent(m_header);
    headerFill->SetOpacity(0.5f);
    headerFill->SetLayer(LAYER_UI_BACKGROUND);

    //--------------------------------------------------------------------------
    // Content

    m_content = new UIObject(scene);
    m_content->SetName("UI Stage Content panel");
    m_content->SetParent(this);
    m_content->GetLocalRect().anchorMin = { 0.f, 0.f };
    m_content->GetLocalRect().anchorMax = { 1.f, 1.f - m_ratioHeader };

    //--------------------------------------------------------------------------
    // Title text

    font = assets->GetFont(FONT_LARGE);
    m_titleText = new UIText(scene, "Stage selection", font, Colors::Gold);
    m_titleText->SetParent(m_header);
    m_titleText->SetAnchor(Anchor::CENTER);
    
    //--------------------------------------------------------------------------
    // Player text

    font = assets->GetFont(FONT_LARGE);
    m_playerText = new UIText(scene, "Fighter selection", font, Colors::Black);
    m_playerText->SetAnchor(Anchor::WEST);

    //--------------------------------------------------------------------------
    // Options text

    font = assets->GetFont(FONT_LARGE);
    m_optionText = new UIText(scene, "Options", font, Colors::Black);
    m_optionText->SetAnchor(Anchor::WEST);

    //--------------------------------------------------------------------------
    // Cursor

    UIDefaultCursor *cursor = new UIDefaultCursor(scene);
    m_group->SetCursor(cursor);

    //--------------------------------------------------------------------------
    // Player list

    std::string labelString;
    std::vector<std::string> textStrings;
    const float textWidth = 95.f;

    // Player 1
    labelString.assign("Player 1");
    textStrings.push_back("Fire knight");
    textStrings.push_back("Water priestess");
    textStrings.push_back("Metal Bladekeeper");
    m_player1List = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_player1List->SetIsCycle(true);
    m_player1List->AddSelectableListener(this);

    // Player 2
    labelString.assign("Player 2");
    m_player2List = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_player2List->SetIsCycle(true);
    m_player2List->AddSelectableListener(this);

    // is AI 
    labelString.assign("Player 2 mode");
    textStrings.clear();
    textStrings.push_back("Player");
    textStrings.push_back("AI");
    m_AIList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_AIList->SetIsCycle(true);
    m_AIList->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Stage duration list

    labelString.assign("Stage duration");
    textStrings.clear();
    textStrings.push_back("1 min");
    textStrings.push_back("2 min");
    textStrings.push_back("3 min");
    m_timeList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_timeList->SetIsCycle(false);
    m_timeList->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Life count list

    labelString.assign("Life Count");
    textStrings.clear();
    textStrings.push_back("1");
    textStrings.push_back("2");
    textStrings.push_back("3");
    m_liveList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_liveList->SetIsCycle(false);
    m_liveList->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Gamemode list

    labelString.assign("Gamemode");
    textStrings.clear();
    textStrings.push_back("Life");
    textStrings.push_back("Time");
    m_gamemodeList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_gamemodeList->SetIsCycle(true);
    m_gamemodeList->AddSelectableListener(this);

    ////--------------------------------------------------------------------------
    //// TODO : Ajout d'un texte et d'une liste pour la fréquence d'apparition des potions
    labelString.assign("Potions frequency");
    textStrings.clear();
    textStrings.push_back("Never");
    textStrings.push_back("Rarely");
    textStrings.push_back("Sometimes");
    textStrings.push_back("Often");
    m_potionsList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_potionsList->SetIsCycle(false);
    m_potionsList->AddSelectableListener(this);

    // Pour les bombes
    labelString.assign("Bombs frequency");
    textStrings.clear();
    textStrings.push_back("Never");
    textStrings.push_back("Rarely");
    textStrings.push_back("Sometimes");
    textStrings.push_back("Often");
    m_bombsList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_bombsList->SetIsCycle(false);
    m_bombsList->AddSelectableListener(this);


    //--------------------------------------------------------------------------
    // Buttons

    m_startButton = new UIDefaultButton(scene, "Start");
    m_startButton->AddSelectableListener(this);

    m_backButton = new UIDefaultButton(scene, "Back");
    m_backButton->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Grid layouts

    // TODO : Modifier le vLayout pour ajouter un champ potion
    UIGridLayout *vLayout = new UIGridLayout(m_scene, 11, 1); 
    vLayout->SetParent(m_content);
    vLayout->SetAnchor(Anchor::CENTER);
    vLayout->SetSpacing(2.f);
    vLayout->SetRowSpacing(3, 10.f);
    vLayout->SetRowSpacing(8, 20.f);
    vLayout->SetRowSize(22.f);
    vLayout->SetColumnSize(300.f);

    vLayout->AddObject(m_playerText, 0, 0);
    vLayout->AddObject(m_player1List, 1, 0);
    vLayout->AddObject(m_player2List, 2, 0);
    vLayout->AddObject(m_AIList, 3, 0);
    vLayout->AddObject(m_optionText, 4, 0);
    vLayout->AddObject(m_gamemodeList, 5, 0);
    vLayout->AddObject(m_timeList, 6, 0);
    vLayout->AddObject(m_liveList, 6, 0);
    vLayout->AddObject(m_potionsList, 7, 0);
    vLayout->AddObject(m_bombsList, 8, 0);
    // TODO : Ajout de l'objet potion

    UIGridLayout *hLayout = new UIGridLayout(m_scene, 1, 2);
    hLayout->SetSpacing(20.f);
    hLayout->SetAnchor(Anchor::CENTER);
    hLayout->SetColumnSize(100.f);

    hLayout->AddObject(m_backButton, 0, 0);
    hLayout->AddObject(m_startButton, 0, 1);

    // TODO : Modifier pour laisser la place au champ potion
    vLayout->AddObject(hLayout, 9, 0);

    vLayout->Update();
    hLayout->Update();

    //--------------------------------------------------------------------------
    // Fade In/Out et Initialisation

    InitFadeAnim();
    InitPageWithConfig();

    CreateMenu(false);
}


void UIStagePage::CreateMenu(bool menuExist) {

    m_group->RemoveAllSelectables();

    m_group->AddSelectable(m_player1List);
    m_group->AddSelectable(m_player2List);
    m_group->AddSelectable(m_AIList);
    m_group->AddSelectable(m_gamemodeList);

    switch (g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:
    m_group->AddSelectable(m_liveList);

        m_timeList->SetEnabled(false);

        m_liveList->SetEnabled(true);

        break;

    case StageConfig::Mode::LIMITED_TIME:

        m_timeList->SetEnabled(true);

    m_group->AddSelectable(m_timeList);
        m_liveList->SetEnabled(false);
        break;
    }

    m_group->AddSelectable(m_potionsList);
    m_group->AddSelectable(m_bombsList);
    m_group->AddSelectable(m_backButton);
    m_group->AddSelectable(m_startButton);
    // TODO : Ajouter la potion


    m_group->ComputeAutoNavigation();
    if (menuExist)
        m_group->SetSelected(m_gamemodeList);
    else
        m_group->SetSelected(m_player1List);
    m_group->SetCursorOnSelected();
}

UIStagePage::~UIStagePage()
{
}

void UIStagePage::Update()
{
    UIObject::Update();

    if (IsFadingIn() || IsFadingOut()) return;

    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    AssetManager *assets = m_scene->GetAssetManager();
    InputManager *inputManager = m_scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);

    for (const UIInput &input : applicationInput->uiInputs)
    {
        if (input.start)
        {
            m_scene->GetSceneManager()->QuitScene();

            assets->PlaySoundFX(SFX_UI_VALIDATE);

            UpdateConfigs();
            m_group->SetSelected(nullptr);
            m_group->SetInteractable(false);

            titleManager->QuitScene();
        }
    }
}

void UIStagePage::OnClick(UISelectable *which)
{
    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    if (which == m_startButton)
    {
        UpdateConfigs();
        m_group->SetSelected(nullptr);
        m_group->SetInteractable(false);

        titleManager->QuitScene();
    }
    else if (which == m_backButton)
    {
        UpdateConfigs();
        PlayFadeOut();
        m_group->SetSelected(nullptr);
        m_group->SetInteractable(false);

        titleManager->OpenPage(TitleManager::Page::TITLE);
    }
}

void UIStagePage::OnItemChanged(UISelectable *which, int itemIdx, int prevItemIdx, bool increase)
{
    UpdateConfigs();

    if (which == m_gamemodeList)
        CreateMenu(true);
}

void UIStagePage::OnFadeOutEnd(UIObject *which)
{
    if (which != this) return;

    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    titleManager->QuitPage(TitleManager::Page::STAGE);
}

void UIStagePage::CreateGroup()
{
}

void UIStagePage::InitFadeAnim()
{
    UIFadeDef fadeDef;
    fadeDef.Reset(1.f);
    fadeDef.fadeOpacity = true;

    SetFadeInAnimation(fadeDef);
    SetFadeOutAnimation(fadeDef);
    SetFadeChildren(false);
}

void UIStagePage::InitPageWithConfig()
{
    switch (g_gameCommon.playerConfigs[0].type)
    {
    default:
    case PlayerType::FIRE_KNIGHT: 
        m_player1List->SetFirstSelectedItem(0); break;
    case PlayerType::WATER_PRIESTESS: 
        m_player1List->SetFirstSelectedItem(1); break;
    case PlayerType::METAL_BLADEKEEPER:
        m_player1List->SetFirstSelectedItem(2); break;
    }

    switch (g_gameCommon.playerConfigs[1].type)
    {
    default:
    case PlayerType::FIRE_KNIGHT: 
        m_player2List->SetFirstSelectedItem(0); break;
    case PlayerType::WATER_PRIESTESS: 
        m_player2List->SetFirstSelectedItem(1); break;
    case PlayerType::METAL_BLADEKEEPER:
        m_player1List->SetFirstSelectedItem(2); break;
    }

    switch (g_gameCommon.stageConfig.mode)
    {
    default:
    case StageConfig::Mode::LIMITED_LIVES:
        m_gamemodeList->SetFirstSelectedItem(0); break;
    case StageConfig::Mode::LIMITED_TIME:
        m_gamemodeList->SetFirstSelectedItem(1); break;
    }

    switch (g_gameCommon.stageConfig.duration)
    {
    default:
    case  1: m_timeList->SetFirstSelectedItem(0); break;
    case  2: m_timeList->SetFirstSelectedItem(1); break;
    case  3: m_timeList->SetFirstSelectedItem(2); break;
    }

    switch (g_gameCommon.stageConfig.lifeCount)
    {
    default:
    case  1: m_liveList->SetFirstSelectedItem(0); break;
    case  2: m_liveList->SetFirstSelectedItem(1); break;
    case  3: m_liveList->SetFirstSelectedItem(2); break;
    }

    switch (g_gameCommon.stageConfig.potionFrequency)
    {
    default:
    case  StageConfig::Frequency::NEVER:
        m_potionsList->SetFirstSelectedItem(0); break;
    case  StageConfig::Frequency::RARELY:
        m_potionsList->SetFirstSelectedItem(1); break;
    case  StageConfig::Frequency::SOMETIMES:
        m_potionsList->SetFirstSelectedItem(2); break;
    case  StageConfig::Frequency::OFTEN:
        m_potionsList->SetFirstSelectedItem(3); break;
    
    }
    // pareil avec les bombes
    switch (g_gameCommon.stageConfig.bombsFrequency)
    {
    default:
    case  StageConfig::Frequency::NEVER:
        m_bombsList->SetFirstSelectedItem(0); break;
    case  StageConfig::Frequency::RARELY:
        m_bombsList->SetFirstSelectedItem(1); break;
    case  StageConfig::Frequency::SOMETIMES:
        m_bombsList->SetFirstSelectedItem(2); break;
    case  StageConfig::Frequency::OFTEN:
        m_bombsList->SetFirstSelectedItem(3); break;

    }
}

void UIStagePage::UpdateConfigs()
{
    switch (m_player1List->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.playerConfigs[0].type = PlayerType::FIRE_KNIGHT; break;
    case 1: g_gameCommon.playerConfigs[0].type = PlayerType::WATER_PRIESTESS; break;
    case 2: g_gameCommon.playerConfigs[0].type = PlayerType::METAL_BLADEKEEPER; break;
    }

    switch (m_player2List->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.playerConfigs[1].type = PlayerType::FIRE_KNIGHT; break;
    case 1: g_gameCommon.playerConfigs[1].type = PlayerType::WATER_PRIESTESS; break;
    case 2: g_gameCommon.playerConfigs[0].type = PlayerType::METAL_BLADEKEEPER; break;
    }

    switch (m_AIList->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.playerConfigs[1].isCPU = false; break;
    case 1: g_gameCommon.playerConfigs[1].isCPU = true; break;
    }
	
    switch (m_gamemodeList->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.stageConfig.mode = StageConfig::Mode::LIMITED_LIVES; break;
    case 1: g_gameCommon.stageConfig.mode = StageConfig::Mode::LIMITED_TIME; break;
    }

    switch (m_timeList->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.stageConfig.duration = 1; break;
    case 1: g_gameCommon.stageConfig.duration = 2; break;
    case 2: g_gameCommon.stageConfig.duration = 3; break;
    }

    switch (m_liveList->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.stageConfig.lifeCount = 1; break;
    case 1: g_gameCommon.stageConfig.lifeCount = 2; break;
    case 2: g_gameCommon.stageConfig.lifeCount = 3; break;
    }

    switch (m_potionsList->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.stageConfig.potionFrequency = StageConfig::Frequency::NEVER; break;
    case 1: g_gameCommon.stageConfig.potionFrequency = StageConfig::Frequency::RARELY; break;
    case 2: g_gameCommon.stageConfig.potionFrequency = StageConfig::Frequency::SOMETIMES; break;
    case 3: g_gameCommon.stageConfig.potionFrequency = StageConfig::Frequency::OFTEN; break;
    }

    //avec les bombes
    
    switch (m_bombsList->GetSelectedItem())
    {
    default:
    case 0: g_gameCommon.stageConfig.bombsFrequency = StageConfig::Frequency::NEVER; break;
    case 1: g_gameCommon.stageConfig.bombsFrequency = StageConfig::Frequency::RARELY; break;
    case 2: g_gameCommon.stageConfig.bombsFrequency = StageConfig::Frequency::SOMETIMES; break;
    case 3: g_gameCommon.stageConfig.bombsFrequency = StageConfig::Frequency::OFTEN; break;
    }
	
    g_gameCommon.UpdatePlayerConfigs();

    // TODO : Gérer la fréquence de la potion
}
