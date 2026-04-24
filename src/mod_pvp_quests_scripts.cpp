/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

enum QuestIds
{
    QUEST_AB_A = 11335,
    QUEST_AB_H = 11339,

    QUEST_AV_A = 11336,
    QUEST_AV_H = 11340,

    QUEST_EOS_A = 11337,
    QUEST_EOS_H = 11341,

    QUEST_WSG_A = 11338,
    QUEST_WSG_H = 11342,

    QUEST_MARKER_WIN = 50010,
    QUEST_MARKER_DEFEAT = 50011,

    QUEST_ARENA_DAILY       = 50012, // Games completed
    QUEST_ARENA_WEEKLY      = 50013, // Games won
    QUEST_ARENA_DAILY_2V2   = 50014,
    QUEST_ARENA_WEEKLY_2V2  = 50015,
    QUEST_ARENA_DAILY_3V3   = 50016,
    QUEST_ARENA_WEEKLY_3V3  = 50017,
    QUEST_ARENA_DAILY_5V5   = 50018,
    QUEST_ARENA_WEEKLY_5V5  = 50019,
    QUEST_ARENA_DAILY_1V1   = 50020,
    QUEST_ARENA_WEEKLY_1V1  = 50021,
    QUEST_ARENA_DAILY_SOLO  = 50022,
    QUEST_ARENA_WEEKLY_SOLO = 50023
};

enum CreatureIds
{
    CREATURE_ARENA_COMPLETED      = 500000,
    CREATURE_ARENA_WON            = 500001,
    CREATURE_ARENA_2V2_COMPLETED  = 500002,
    CREATURE_ARENA_2V2_WON        = 500003,
    CREATURE_ARENA_3V3_COMPLETED  = 500004,
    CREATURE_ARENA_3V3_WON        = 500005,
    CREATURE_ARENA_5V5_COMPLETED  = 500006,
    CREATURE_ARENA_5V5_WON        = 500007,
    CREATURE_ARENA_1V1_COMPLETED  = 500008,
    CREATURE_ARENA_1V1_WON        = 500009,
    CREATURE_ARENA_SOLO_COMPLETED = 500010,
    CREATURE_ARENA_SOLO_WON       = 500011
};

enum ArenaTypeIds
{
    ARENA_TYPE_1V1 = 1,
    ARENA_TYPE_2V2 = 2,
    ARENA_TYPE_3V3 = 3,
    ARENA_TYPE_SOLO = 4, // 3v3 solo queue
    ARENA_TYPE_5V5 = 5
};

static void GiveBracketQuestCredit(Player* player, uint8 arenaType, bool winner)
{
    switch (arenaType)
    {
    case ARENA_TYPE_2V2:
        if (player->HasQuest(QUEST_ARENA_DAILY_2V2))
            player->KilledMonsterCredit(CREATURE_ARENA_2V2_COMPLETED);

        if (player->HasQuest(QUEST_ARENA_WEEKLY_2V2) && winner)
            player->KilledMonsterCredit(CREATURE_ARENA_2V2_WON);
        break;
    case ARENA_TYPE_3V3:
        if (player->HasQuest(QUEST_ARENA_DAILY_3V3))
            player->KilledMonsterCredit(CREATURE_ARENA_3V3_COMPLETED);

        if (player->HasQuest(QUEST_ARENA_WEEKLY_3V3) && winner)
            player->KilledMonsterCredit(CREATURE_ARENA_3V3_WON);
        break;
    case ARENA_TYPE_5V5:
        if (player->HasQuest(QUEST_ARENA_DAILY_5V5))
            player->KilledMonsterCredit(CREATURE_ARENA_5V5_COMPLETED);

        if (player->HasQuest(QUEST_ARENA_WEEKLY_5V5) && winner)
            player->KilledMonsterCredit(CREATURE_ARENA_5V5_WON);
        break;
        // Custom cases
    case ARENA_TYPE_1V1:
        if (player->HasQuest(QUEST_ARENA_DAILY_1V1))
            player->KilledMonsterCredit(CREATURE_ARENA_1V1_COMPLETED);

        if (player->HasQuest(QUEST_ARENA_WEEKLY_1V1) && winner)
            player->KilledMonsterCredit(CREATURE_ARENA_1V1_WON);
        break;
    case ARENA_TYPE_SOLO:
        if (player->HasQuest(QUEST_ARENA_DAILY_SOLO))
            player->KilledMonsterCredit(CREATURE_ARENA_SOLO_COMPLETED);

        if (player->HasQuest(QUEST_ARENA_WEEKLY_SOLO) && winner)
            player->KilledMonsterCredit(CREATURE_ARENA_SOLO_WON);
        break;
    }
}

class BgQuestRewardScript : public BGScript
{
public:
    BgQuestRewardScript() : BGScript("mod_bg_quest_reward_script", {
        ALLBATTLEGROUNDHOOK_ON_BATTLEGROUND_END_REWARD
    }) { }

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId winnerTeamId) override
    {
        if (!sConfigMgr->GetOption<bool>("ModPvPQuests.Enable", true))
            return;

        bool winner = (player->GetBgTeamId() == winnerTeamId);

        if (player->GetMap()->IsBattleArena())
        {
            if (player->IsSpectator())
                return;

            if (!bg->isRated())
                return; // Skirmish (unrated arena match)

            if (bg->GetStartDelayTime())
                return; // Match hasn't started yet

            if (bg->GetStartTime() < 75 * IN_MILLISECONDS)
                return; // 15 seconds haven't passed after match start (starts at 60s)

            if (player->HasQuest(QUEST_ARENA_DAILY))
                player->KilledMonsterCredit(CREATURE_ARENA_COMPLETED);

            if (player->HasQuest(QUEST_ARENA_WEEKLY) && winner)
                player->KilledMonsterCredit(CREATURE_ARENA_WON);

            GiveBracketQuestCredit(player, bg->GetArenaType(), winner);

            return;
        }

        if (winner)
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_MARKER_WIN))
            {
                if (player->CanTakeQuest(quest, false))
                {
                    player->AddQuest(quest, nullptr);
                    player->CompleteQuest(QUEST_MARKER_WIN);
                    player->RewardQuest(quest, 0, nullptr, true, true);

                    if (int32 ap = sConfigMgr->GetOption<int>("ModPvPQuests.WinAP", 10))
                        player->ModifyArenaPoints(ap);
                }
            }
        }
        else
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_MARKER_DEFEAT))
            {
                if (int32 ap = sConfigMgr->GetOption<int>("ModPvPQuests.LossAP", 0))
                {
                    if (player->CanTakeQuest(quest, false))
                    {
                        player->AddQuest(quest, nullptr);
                        player->CompleteQuest(QUEST_MARKER_DEFEAT);
                        player->RewardQuest(quest, 0, nullptr, true, true);
                        player->ModifyArenaPoints(ap);
                    }
                }
            }
        }

    }
};

// Add all scripts in one
void ModPvPQuestsScripts()
{
    new BgQuestRewardScript();
}
