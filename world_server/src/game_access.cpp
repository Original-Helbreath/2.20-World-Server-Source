// The legacy game's side of the World host (WORLD-005). Built with the game's settings (it includes Game.h); NOMINMAX
// is set for this file so Schemata's headers keep std::min and std::max.
#include "game_access.h"

#include "log.h"
#include "player_mapping.h"

#include "Game.h"

#include <format>
#include <utility>

#include "pending_limits.h"

extern class CGame * G_pGame;  // main.cpp

static_assert(hb::server::world::min_game_message_bytes == DEF_INDEX2_MSGTYPE + 2, "game message minimum is id + type");
static_assert(hb::server::world::max_game_message_bytes == DEF_MSGBUFFERSIZE, "game message maximum is the legacy buffer");

namespace hb::server::world
{
	namespace
	{
		// The per-item fix-ups the 2.20 file loader made after reading an item (Game.cpp, FILE_MODE decoder), in the
		// same order. Returns the item's price when it is dropped (no longer in use, or a duplicated ID), else 0.
		int finish_loaded_item(CGame& game, int iClientH, CItem *& pItem, BOOL bInBag, int iBagIndex, BOOL & bNotUsedFound)
		{
			CClient & c = *game.m_pClientList[iClientH];

			if (bInBag == TRUE) {
				// If the weight is exceeded, reduce to 1.
				if (game.iGetItemWeight(pItem, (int)pItem->m_dwCount) > game._iCalcMaxLoad(iClientH)) {
					pItem->m_dwCount = 1;
					hb::log::warning("character {} item {} count overflow", c.m_cCharName, pItem->m_cName);
				}
			}
			// v1.41 Custom-made: the max lifespan is ItemSpecialEffectValue1.
			if ((pItem->m_dwAttribute & 0x00000001) != 0)
				pItem->m_wMaxLifeSpan = pItem->m_sItemSpecEffectValue1;
			// v2.16 Upgraded shields and armour: the max lifespan is ItemSpecialEffectValue1.
			if (((pItem->m_dwAttribute & 0xF0000000) >> 28) > 0) {
				switch (pItem->m_cCategory) {
				case 5:
				case 6:
					pItem->m_wMaxLifeSpan = pItem->m_sItemSpecEffectValue1;
					break;
				}
			}
			// v1.42 Rare items reset their max lifespan, speed, weight and so on.
			game._AdjustRareItemValue(pItem);
			if (pItem->m_wCurLifeSpan > pItem->m_wMaxLifeSpan) pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
			// v1.433 A Stone of Sacrifice never has lifespan 0.
			if ((pItem->m_wCurLifeSpan == 0) && (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)) {
				hb::log::warning("character {} held a Stone of Sacrifice with lifespan 0", c.m_cCharName);
				pItem->m_wCurLifeSpan = 1;
			}
			// v2.05 +damage items to the new stat info.
			if (bInBag == TRUE) game.bCheckAndConvertPlusWeaponItem(iClientH, iBagIndex);

			if (pItem->m_cItemType == DEF_ITEMTYPE_NOTUSED) {
				const int iPrice = pItem->m_wPrice;
				delete pItem;
				pItem = NULL;
				if (bInBag == TRUE) bNotUsedFound = TRUE;
				return iPrice;
			}
			if (game._bCheckDupItemID(pItem) == TRUE) {
				game._bItemLog(DEF_ITEMLOG_DUPITEMID, iClientH, NULL, pItem);
				const int iPrice = pItem->m_wPrice;
				delete pItem;
				pItem = NULL;
				return iPrice;
			}
			return 0;
		}

		bool load_into_slot(CGame& game, int iClientH, std::int64_t account_id, const character_record& record, std::string& error)
		{
			CClient & c = *game.m_pClientList[iClientH];
			apply_character(c, record);
			c.m_iAccountID = account_id;
			// The account name only has to be unique and valid for the legacy checks that compare it; the Login server
			// owns the real one.
			const std::string account_text = std::format("{}", account_id);
			ZeroMemory(c.m_cAccountName, sizeof(c.m_cAccountName));
			memcpy(c.m_cAccountName, account_text.data(), (account_text.size() < 10) ? account_text.size() : 10);
			c.m_iHitRatio = 0;
			c.m_iDefenseRatio = 0;
			c.m_cSide = 0;

			for (int i = 0; i < DEF_MAXMAPS; i++)
				if ((game.m_pMapList[i] != NULL) && (memcmp(game.m_pMapList[i]->m_cName, c.m_cMapName, 10) == 0))
					c.m_cMapIndex = (char)i;
			if (c.m_cMapIndex == -1) {
				error = std::format("map {} is not loaded", c.m_cMapName);
				return false;
			}

			const item_factory make = [&game](const character_item& stored) -> CItem * {
				CItem * pItem = new class CItem;
				std::string name = stored.name;
				if (game._bInitItemAttr(pItem, name.data()) == FALSE) {
					delete pItem;
					return NULL;
				}
				apply_item_values(*pItem, stored);
				return pItem;
			};
			if (!apply_items(c, record, make, error)) return false;

			int iNotUsedItemPrice = 0;
			BOOL bNotUsedFound = FALSE;
			for (int i = 0; i < DEF_MAXITEMS; i++)
				if (c.m_pItemList[i] != NULL)
					iNotUsedItemPrice += finish_loaded_item(game, iClientH, c.m_pItemList[i], TRUE, i, bNotUsedFound);
			for (int i = 0; i < DEF_MAXBANKITEMS; i++)
				if (c.m_pItemInBankList[i] != NULL)
					iNotUsedItemPrice += finish_loaded_item(game, iClientH, c.m_pItemInBankList[i], FALSE, i, bNotUsedFound);

			// The equip state, last, as the file loader did: an item is worn only if it is equipment and can still be
			// worn with these stats. A dropped unused item voids every equip flag (v1.41). No notices: the player is
			// not in the game yet.
			for (int i = 0; i < DEF_MAXITEMEQUIPPOS; i++) c.m_sItemEquipmentStatus[i] = -1;
			for (int i = 0; i < DEF_MAXITEMS; i++) {
				const BOOL bWanted = (bNotUsedFound == FALSE) && (c.m_bIsItemEquipped[i] == TRUE) && (c.m_pItemList[i] != NULL)
					&& (c.m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_EQUIP);
				c.m_bIsItemEquipped[i] = bWanted;
				if ((bWanted == TRUE) && (game.bEquipItemHandler(iClientH, (short)i, FALSE) == FALSE))
					c.m_bIsItemEquipped[i] = FALSE;
			}

			if (game._bFinishDecodedPlayer(iClientH, iNotUsedItemPrice) == FALSE) {
				error = "character data failed the load checks";
				return false;
			}
			return true;
		}
	}

	game_state game_entry_state()
	{
		CGame * game = G_pGame;
		if (game == NULL) return game_state::not_ready;
		if (game->m_bOnExitProcess == TRUE) return game_state::shutting_down;
		if ((game->m_bIsGameStarted == FALSE) || (game->m_bIsLoginLinkAvailable == FALSE)) return game_state::not_ready;
		if (game->m_iTotalClients >= DEF_MAXCLIENTS - 1) return game_state::full;
		return game_state::accepting;
	}

	int game_prepare_player(std::unique_ptr<world_client_socket> socket, std::int64_t account_id, const character_record& record,
		std::string& error)
	{
		error.clear();
		CGame * game = G_pGame;
		if (game == NULL) {
			error = "the game is not running";
			return -1;
		}
		const int iClientH = game->iAddEnteredClient(socket.get());
		if (iClientH < 0) {
			error = "no client slot (World not ready or full)";
			return -1;
		}
		(void)socket.release();  // the client owns it now
		if (!load_into_slot(*game, iClientH, account_id, record, error)) {
			// The host tells the Login server (session_ended disconnected) for every refusal; not twice from here.
			game->m_pClientList[iClientH]->m_iAccountID = 0;
			game->DeleteClient(iClientH, FALSE, FALSE);
			return -1;
		}
		return iClientH;
	}

	void game_replace_account(std::int64_t account_id, std::string_view character)
	{
		CGame * game = G_pGame;
		if (game == NULL) return;
		// One player per account and per character: an older session still in the game is saved and removed. Its
		// session_ended (kicked) goes out now, before the new session_started, which is the order the Login server
		// needs (its on_ended erases the account's entry whatever its state; on_started then records the new one).
		const std::string name(character);
		for (int i = 1; i < DEF_MAXCLIENTS; i++) {
			CClient * other = game->m_pClientList[i];
			if (other == NULL) continue;
			if ((other->m_iAccountID == account_id) || (_strnicmp(other->m_cCharName, name.c_str(), 10) == 0)) {
				hb::log::info("<{}> {} replaced by a new entry", i, other->m_cCharName);
				other->m_cSessionEndReason = 2;  // session_end_reason::kicked
				game->DeleteClient(i, TRUE, TRUE);
			}
		}
	}

	void game_finish_player(int client_index)
	{
		CGame * game = G_pGame;
		if ((game == NULL) || (client_index <= 0) || (client_index >= DEF_MAXCLIENTS) || (game->m_pClientList[client_index] == NULL)) return;
		// No guild check exists yet (the guild files went with the log server): the guild is taken as still there.
		game->_FinishInitPlayer(client_index, 1);
	}

	void game_client_message(int client_index, const std::vector<std::byte>& message)
	{
		// The legacy handlers read the message id and type unchecked; the gate refuses shorter bodies, and so does this.
		if ((G_pGame == NULL) || !is_acceptable_game_message(message.size())) {
			hb::log::warning("<{}> game message of {} bytes dropped", client_index, message.size());
			return;
		}
		std::vector<std::byte> copy = message;
		G_pGame->OnClientMessage(client_index, reinterpret_cast<char *>(copy.data()), (DWORD)copy.size());
	}

	void game_client_closed(int client_index)
	{
		if (G_pGame != NULL) G_pGame->OnClientClosed(client_index);
	}

	bool game_capture_player(int client_index, character_record& record)
	{
		CGame * game = G_pGame;
		if ((game == NULL) || (client_index <= 0) || (client_index >= DEF_MAXCLIENTS)) return false;
		CClient * c = game->m_pClientList[client_index];
		if ((c == NULL) || (c->m_bIsInitComplete == FALSE) || (c->m_iAccountID == 0)) return false;
		// As the 2.20 file writer did: a dead character is saved at full HP.
		if (c->m_iHP <= 0) c->m_iHP = game->iGetMaxHP(client_index);
		capture_character(*c, record);
		return true;
	}

	void game_remove_account(std::int64_t account_id, std::uint8_t end_reason)
	{
		CGame * game = G_pGame;
		if (game == NULL) return;
		for (int i = 1; i < DEF_MAXCLIENTS; i++)
			if ((game->m_pClientList[i] != NULL) && (game->m_pClientList[i]->m_iAccountID == account_id)) {
				game->m_pClientList[i]->m_cSessionEndReason = (char)end_reason;
				game->DeleteClient(i, TRUE, TRUE);
			}
	}

	void game_remove_all(std::uint8_t end_reason)
	{
		CGame * game = G_pGame;
		if (game == NULL) return;
		for (int i = 1; i < DEF_MAXCLIENTS; i++)
			if (game->m_pClientList[i] != NULL) {
				game->m_pClientList[i]->m_cSessionEndReason = (char)end_reason;
				game->DeleteClient(i, TRUE, FALSE);
			}
	}

	void game_for_each_player(const std::function<void(std::int64_t account_id, std::string_view character)>& visit)
	{
		CGame * game = G_pGame;
		if (game == NULL) return;
		for (int i = 1; i < DEF_MAXCLIENTS; i++) {
			CClient * c = game->m_pClientList[i];
			if ((c != NULL) && (c->m_bIsInitComplete == TRUE) && (c->m_iAccountID != 0))
				visit(c->m_iAccountID, std::string_view(c->m_cCharName, strnlen(c->m_cCharName, sizeof(c->m_cCharName))));
		}
	}
}
