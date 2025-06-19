/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "common/game_settings.h"
#include "common/game_common.h"
#include "ui/custom_default/ui_default_button.h"
#include "ui/custom_default/ui_text_list.h"

class UIStagePage : public UIObject, public UISelectableListener
{
public:
    UIStagePage(Scene *scene);
    virtual ~UIStagePage();

    virtual void Update() override;

    virtual void OnClick(UISelectable *which) override;
    virtual void OnItemChanged(
        UISelectable *which, int itemIdx, int prevItemIdx, bool increase);

protected:
    virtual void OnFadeOutEnd(UIObject *which) override;
    void CreateGroup();
private:
    float m_ratioHeader;
    UIObject *m_header;
    UIObject *m_content;

    UIText *m_titleText;
    UIText *m_playerText;
    UIText *m_optionText;
    // TODO : texte pour la potion
    UIText* m_itemsText;
    UITextList *m_player1List;
    UITextList *m_player2List;
    UITextList *m_AIList;
    UITextList *m_timeList;
    UITextList *m_liveList;
    UITextList *m_gamemodeList;
    // TODO : Liste de fréquence pour la potion
    UITextList *m_potionsList;
    UITextList *m_bombsList;

    UIDefaultButton *m_startButton;
    UIDefaultButton *m_backButton;

    UISelectableGroup *m_group;

    void InitFadeAnim();
    void InitPageWithConfig();
    void UpdateConfigs();
    void CreateMenu(bool menuExist);
};

