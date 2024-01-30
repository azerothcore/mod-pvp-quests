/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

enum QuestIds
{
    QUEST_AB_A  = 11335,
    QUEST_AB_H  = 11339,

    QUEST_AV_A  = 11336,
    QUEST_AV_H  = 11340,

    QUEST_EOS_A = 11337,
    QUEST_EOS_H = 11341,

    QUEST_WSG_A = 11338,
    QUEST_WSG_H = 11342
};

class BgQuestRewardScript : public BGScript
{
public:
    BgQuestRewardScript() : BGScript("mod_bg_quest_reward_script") { }

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId winnerTeamId) override
    {
        if (winnerTeamId == player->GetBgTeamId())
        {
            switch (bg->GetMapId())
            {
                case 30:  // AV
                    if (player->GetTeamId(true) == TEAM_ALLIANCE)
                    {
                        if (player->CanCompleteQuest(QUEST_AV_A))
                        {
                            player->CompleteQuest(QUEST_AV_A);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_AV_A))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    else
                    {
                        if (player->CanCompleteQuest(QUEST_AV_H))
                        {
                            player->CompleteQuest(QUEST_AV_H);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_AV_H))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    break;
                case 489: // WSG
                    if (player->GetTeamId(true) == TEAM_ALLIANCE)
                    {
                        if (player->CanCompleteQuest(QUEST_WSG_A))
                        {
                            player->CompleteQuest(QUEST_WSG_A);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_WSG_A))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    else
                    {
                        if (player->CanCompleteQuest(QUEST_WSG_H))
                        {
                            player->CompleteQuest(QUEST_WSG_H);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_WSG_H))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    break;
                case 529: // AB
                    if (player->GetTeamId(true) == TEAM_ALLIANCE)
                    {
                        if (player->CanCompleteQuest(QUEST_AB_A))
                        {
                            player->CompleteQuest(QUEST_AB_A);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_AB_A))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    else
                    {
                        if (player->CanCompleteQuest(QUEST_AB_H))
                        {
                            player->CompleteQuest(QUEST_AB_H);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_AB_H))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    break;
                case 566: // EOTS
                    if (player->GetTeamId(true) == TEAM_ALLIANCE)
                    {
                        if (player->CanCompleteQuest(QUEST_EOS_A))
                        {
                            player->CompleteQuest(QUEST_EOS_A);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_EOS_A))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    else
                    {
                        if (player->CanCompleteQuest(QUEST_EOS_H))
                        {
                            player->CompleteQuest(QUEST_EOS_H);
                            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_EOS_H))
                            {
                                player->RewardQuest(quest, 0, nullptr, true, true);
                            }
                        }
                    }
                    break;
            }
        }
    }
};

// Add all scripts in one
void ModPvPQuestsScripts()
{
    new BgQuestRewardScript();
}
