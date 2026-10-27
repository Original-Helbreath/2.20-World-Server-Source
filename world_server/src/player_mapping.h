#pragma once

// players.db <-> the game's CClient (WORLD-005): the typed replacement for the log server's character file
// (_bDecodePlayerDatafileContents / _iComposePlayerDataFileContents). Field for field; the game's own rules for a
// loaded character (item fix-ups, equipping, the checks in CGame::_bFinishDecodedPlayer) run in game_access.cpp,
// which needs the whole game. These functions need only CClient and CItem, so the tests run them alone.

#include "character_store.h"

#include <functional>

class CClient;
class CItem;

namespace hb::server::world
{
	// Every stored character value, skill and magic onto a freshly constructed client. The map index is left at -1
	// for the game to resolve; account and character ids are kept for the save.
	void apply_character(CClient& client, const character_record& record);

	// One stored item's own values onto an item the game has already set up from its config by name.
	void apply_item_values(CItem& item, const character_item& stored);

	// Makes the item for one stored row, set up from the game's config and with apply_item_values done; null when the
	// name is not a known item.
	using item_factory = std::function<CItem*(const character_item&)>;

	// Places the bag items in slot order at indexes 0, 1, 2 ... (with their positions and the stored equip flag, which
	// the game's equip pass then checks) and the bank items likewise. False, with the item named in error, when a bag
	// or bank is over its size or an item is unknown; the client is then left for the caller to delete.
	bool apply_items(CClient& client, const character_record& record, const item_factory& make, std::string& error);

	// The client as a record: header (with its ids), bag then bank items with slots numbered from 0 in index order,
	// skills with any mastery or SSN, and magic with any mastery. Values are normalised as the 2.20 file writer did
	// (no guild: name NONE and GUID -1; negative SP and contribution saved as 0).
	void capture_character(const CClient& client, character_record& record);
}
