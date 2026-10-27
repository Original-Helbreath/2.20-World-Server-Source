//#define DEF_WEAPONSPEEDLIMIT	// v2.172
#define DEF_MAXCRUSADESTRUCTURES		300
#define DEF_MAXAGRICULTURE				200		//v2.19 2002-12-16


//#define NO_MSGSPEEDCHECK		// Option to skip the client speed check
//#define DEF_CHECKUSERNPCNUM	// Option to spawn monsters in proportion to users
//#define DEF_GUILDWARMODE		// Guild war mode: some spells cannot be used.
//#define DEF_DBGAMESERVER      // DB game server mode
//#define DEF_NPCITEM			// NPC item drop chances come from the config file.
//#define DEF_BEGINNEREVENTMODE // Beginner event mode (double experience in the beginner middleland)

//#define DEF_TESTSERVER		// Test server mode
//#define DEF_KOREA
//#define DEF_TAIWAN
#define DEF_CHINESE
//#define DEF_JAPAN
//#define DEF_USA
//#define DEF_INTERSERVER


// Common to all countries
//#define	DEF_WINTER	// Winter setting (makes it snow.)
#define DEF_V219
#define DEF_BEGINNERTOWNPROTECT	// No attacks in neutral towns v2.181

#ifdef DEF_KOREA
	#define DEF_CONSULTATION		// Content rating related
	#define DEF_DEFENCEHACKING		// 2002-12-8 For World server security settings
	#define DEF_CHINESECANDYEVENT	// 2002-12-12 China candy event
#endif

#if defined(DEF_INTERSERVER)
	#define DEF_MAXLEVEL150
	#define DEF_DBGAMESERVER	// DB game server mode
	#define DEF_DEFENCEHACKING  // 2002-12-8 For World server security settings
	#define DEF_ENGLISHITEMNAME		// v2.15 Needs setting again later.. postponed for lack of time
	#define DEF_LOCALNPCNAME		// v2.14 For Chinese NPC names in Taiwan
#endif

#ifdef DEF_TESTSERVER
	#define DEF_DBGAMESERVER		// DB game server mode
	// Under test
	// #define DEF_NPCITEM
#endif 

#ifdef DEF_USA
	#define DEF_SAMESIDETOWNPROTECT	// No attacks between same side in town v2.172
	#define DEF_MAXLEVEL150
//	#define DEF_ITEMDROPRATINGUP	1500
#endif


#ifdef DEF_TAIWAN
	#define DEF_TAIWANLOG
//	#define DEF_DEFENCEHACKING		// 2002-12-8 For World server security settings
	#undef	DEF_WINTER	// Winter setting (makes it snow.)
//	#define DEF_BALLEVENT           // !! Note: Jan 1 ~ 15 Taiwan wish orb event
	#define DEF_DBGAMESERVER		// DB game server mode
	#define DEF_LOCALNPCNAME		// v2.14 For Chinese NPC names in Taiwan
	#define DEF_ENGLISHITEMNAME		// v2.15 Needs setting again later.. postponed for lack of time
	#define DEF_LOGTIME
#endif

#ifdef DEF_CHINESE
	#define DEF_CHINESECANDYEVENT	// 2002-12-12 China candy event
	#define DEF_CHINESEGUILDLIMIT
	#define DEF_NPCITEM
	// #define DEF_ITEMDROPRATINGUP	3000	// dice(1, 10000) > (9000 - 3000)  => item drops
#endif

#if  defined(DEF_CHINESE) ||defined(DEF_JAPAN) || defined (DEF_USA)
	#define DEF_DBGAMESERVER		// DB game server mode
	#define DEF_LOCALNPCNAME		// v2.14 For Chinese NPC names in Taiwan
	#define DEF_ENGLISHITEMNAME		// v2.15 Needs setting again later.. postponed for lack of time
	#define DEF_LOGTIME
	#define DEF_DEFENCEHACKING		// 2002-12-8 For World server security settings
#endif



#ifdef DEF_CONSULTATION
//	#define DEF_ALLPLAYERPROTECT	// v2.19 2002-11-12 Set areas where characters cannot attack each other
	#define DEF_PKPENALTY			// v2.19 2002-11-18 A PK always goes to jail
	#define DEF_SAMESIDETOWNPROTECT	// No attacks between same side in town v2.172
	#define DEF_ITEMDROP			// v2.19 2002-11-14 Only potions and consumables drop. (content rating)
#endif


/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////        Game Sever Fix&Debug  History         ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEBUG 2003-04-20 China item duplication code....

// TODO
// Must be copied to v2.22. Stopped the snow
v2.20 2002-2-22 #define DEF_MAXLEVEL150
v2.20 2002-2-22 #define DEF_ITEMDROPRATINGUP
v2.20 2002-2-22 Raised the item chance
v2.20 2002-2-22 Raised the base all-out war experience.
v2.20 2002-3-3  Fixed because the supreme upgrade used item names



v2.20 2002-12-28 Fixed the 3rd anniversary ring bug
v2.20 2002-12-28 After the all-out war, force-recall civilians to town
v2.20 2002-12-31 Added civilian mode level and area limits
v2.20 2002-12-31 Gaining citizenship sets civilian mode automatically.


2002-12-24 Limit the NPCs (excluding buildings) that can be summoned in all-out war. (Taiwan request)

<< v2.20 patch >>
v2.20 2002-12-28 Fixed the 3rd anniversary ring bug
v2.20 2002-12-28 After the all-out war, force-recall civilians to town
v2.20 2002-12-23 Checks whether magic can be learned here.
v2.20 2002-12-23 Checks whether this is a warehouse.
v2.20 2002-12-20 Helper NPC
v2.20 2002-12-20 A freezing attack.
v2.20 2002-12-20 Possible from farming skill level 20
v2.20 2002-12-20 It always snows at Christmas and on the icebound map.
v2.20 2002-12-20 Town NPCs cannot be attacked

<< v2.19 patch >>
v2.19 2002-12-20 Separate settings for the Taiwan wish orb event
v2.19 2002-12-20 With content rating set, no Stone of Sacrifice drops.
v2.19 2002-12-18 Fixed the bug where the profile did not show
v2.19 2002-12-18 Added map monster items
v2.19 2002-12-16 Farming skill
v2.19 2002-12-14 Items drop when killed by a monster.. (except summoned mobs)
v2.19 2002-12-11 Santa suit lifespan
v2.19 2002-12-7  Supreme items
v2.19 2002-12-7  Santa suit
v2.19 2002-12-6  Added the Teleport feature
v2.19 2002-12-9  Added Rudolph
      2002-12-11	   Added an anti-hacking routine for China

v2.19 2002-12-4 Raise the guild master's experience.
v2.19 2002-12-4 Lowered the construction points gained while building.
v2.19 2002-12-4  Fixed a party bug
v2.19 2002-12-3  Send the mob event location to the client.
v2.19 2002-12-2  In a fight zone other guilds must count as enemies.
v2.19 2002-11-29 Also cure poison on map moves within the same server. (Taiwan request)
v2.19 2002-11-28 [China] Different forced recall times.
v2.19 2002-11-28 [China] Guild creation requires level 100 and 1,000,000 gold
v2.19 2002-10-25 Fixed the 3rd anniversary event ring effect
v2.19 2002-11-25 Fixed a forced recall bug
v2.19 2002-11-25 Civilians cannot cast some support spells on soldiers (SP down, invisibility, detect, summon, arrow/magic protection, berserk, poison)
v2.19 2002-11-19 Added a variable so the beginner middleland can never be attacked.
v2.19 2002-11-18 A PK always goes to jail
v2.19 2002-11-18 Added a variable that checks whether inside a building
v2.19 2002-11-17 A party gives 110% by default, up to 130%.
v2.19 2002-11-17 Attack skills rise faster.
v2.19 2002-11-17 The Merien series is not knocked off by hammers.
v2.19 2002-11-17 Weapons drop more often.
v2.19 2002-11-17 Being in a party raises the item drop chance.
v2.19 2002-11-17 Raised experience for successful mining, alchemy, fishing and weapon crafting.
v2.19 2002-11-17 Possession has no effect if people are nearby.
v2.19 2002-11-17 Summoning works only for the same side.
v2.19 2002-11-15 Added a variable that checks whether the player is a hunter
v2.19 2002-11-15 In all-out war, guild masters of the side that lost the previous war get 3000 summon points..
v2.19 2002-11-14 Adjusted item prices.
v2.19 2002-11-14 Added hunter mode
v2.19 2002-11-14 Only consumables and food drop..
v2.19 2002-11-12 Set areas where characters cannot attack each other
v2.19 2002-11-12 Changed LocationName to m_cName to widen the forced recall area  Set the Location Name as the forced recall area.
v2.19 2002-11-12 Items can be repaired only at the shop or blacksmith (anti-hacking)

<< v2.181 patch >>

v2.181 2002-11-6  Fixed the bug where NPC PKs were not logged

v2.181 2002-11-5  Fixed the bug where game server connections often failed
v2.181 2002-11-4  Fixed a quest bug (reset the quest of users holding a deleted quest.)

v2.181 2002-10-31 Other wish orbs are not created during the 3rd anniversary event.
v2.181 2002-10-31 Level 15 and above only
v2.181 2002-10-31 Also check at the moment citizenship is gained. (adamas)

v2.181 2002-10-22 #1 3rd anniversary event: event wish orbs

// 2.181 patch /////////////////
v2.181 2002-10-24 Fixed entering a building when force-recalled at an enemy town building entrance
v2.181 2002-10-24 Fewer string compares for server speed.
v2.181 2002-10-24 Fixed the bug where neutrals got attacked
v2.181 2002-10-24 Fixed the bug where supreme characters gained less experience
v2.181 2002-10-24 Raised all-out war experience
v2.181 2002-10-25 Added the 3rd anniversary event ring effect
v2.181 2002-10-25 Double experience in the beginner middleland
  
v2.18 2002-10-15 Added a password to important GM commands
// 2002-09-16 patch /////////////////
v2.17 2002-09-18 #1 Changed mob positions for the official patch
v2.17 2002-09-17 #1 Changed the NPCITEM settings
v2.17 2002-09-12 #2 New monster item settings
v2.17 2002-09-12 #1 Set where Mountain-Giant, Ettin and Cannibal-Plant appear
v2.17 2002-09-11 #3 Enemy monsters cannot be made berserk
v2.17 2002-09-11 #2 Raised the drop chance of anti-freeze potion ingredients (werewolf heart, cyclops leather)
v2.17 2002-09-11 #1 +5% chance at alchemy skill 100
v2.17 2002-09-10 #3 No paralysis on the same side except in all-out war and middleland
v2.17 2002-09-10 #2 No paralysis in safe areas.
v2.17 2002-09-10 #1 Enemy town forced recall time changed from 5 to 3 minutes
////////////////////////////////////

v2.17 2002-09-09 #2 Added a debug message file save function (adamas)
v2.17 2002-09-09 #1 Changed NPCITEM settings (old item name -> item ID) (adamas)

v2.17 2002-09-06 #1 NPCITEM settings (adamas)
v2.17 2002-09-05 #3 adamas 6 -> 8 not sure why this was 6. MAXPARTYMEMBER define (adamas)
v2.17 2002-09-05 #2 Log war contribution (when killing a same-side NPC) (adamas)
v2.17 2002-09-05 #1 Reset the top supreme upgrade to 15 (adamas)

v2.17 2002-08-17 Removed the bug where summoned mobs attacked users on another map of the same server (adamas)

v2.17 2002-8-6  Invisibility ends when casting magic (to stop the invisibility hack)
v2.17 2002-8-6  Attackers of level 100+ gain no experience from rabbits or cats.
v2.17 2002-8-5  No items drop from kills by GM command.
v2.17 2002-8-5  GMs take no damage from field spells.
v2.17 2002-8-2  Raised the automatic experience.
v2.17 2002-7-31 Allow creating items by their unique number.
v2.17 2002-7-31 To log every item dropped when a character dies
v2.17 2002-7-31 Added a trace log to detect the sacrifice bug
v2.17 2002-7-31 Fixed: max-level characters lose no experience on death
v2.17 2002-7-31 Crafted items are logged too.
v2.17 2002-7-22 Fixed the /fi bug (sometimes showed someone as online)
v2.17 2002-7-21 No attacks from the same side only in towns and farmland
v2.17 2002-7-21 In all-out war, beginner zones and farmland give full experience.
v2.17 2002-7-21 Show the user why an item upgrade failed.
v2.17 2002-7-15 Made the forced recall time adjustable.
v2.17 2002-7-4  GMs become invisible automatically on login
v2.17 2002-7-4  Changed the fight-zone check so there can be more than 10 fight zones
v2.17 2002-7-4  Let the map set the number of mobs in a mob event.
v2.17 2002-7-4  Lowered the chance of rain.
v2.172 2002-7-4 Hammer speed limit, armour knocked off when hit,
v2.17           No forced recall from fight zones in guild war mode.
v2.17 2002-6-29 Invisibility while mining or playing dead ends when casting magic.
v2.17 2002-7-2  Changed to fix the fist bug
v2.16 2002-6-2  Added handling for negative lifespan
v2.16 2002-6-2  No recall, Bleeding Isle or other tickets in enemy towns
v2.16 2002-5-31 Dying in an enemy town sends the player to jail
v2.16 2002-5-27 Added the underwear change potion
v2.16 2002-5-27 Added hammer and staff criticals and skills
v2.15 2002-5-21 Send the GM Shutup command to all servers
v2.15 2002-5-15 Temporarily no item drops from demons
v2.15 2002-5-15 Robes can be repaired
v2.15 2002-5-15 Added GM items and effects
v2.15 2002-5-6  Added the game server management client connection
v2.15 2002-5-6  Fixed the error message shown when the game server dies
v2.15 2002-5-6  Fixed a supreme item bug
v2.15 2002-4-20 Slightly raised the item drop chance
v2.15 2002-4-20 Fixed the wrong magic success rate
v2.15 2002-4-19 Added a routine to show building HP in all-out war. Note!!! The routine assumes 4 buildings
v2.15 2002-2-29 Added a routine to fix the bow bug
v2.15 2002-2-29 Set the forced disconnect count to 10 when it goes above 3000
v2.15 2002-2-29 Raised base experience for killing enemies
v2.15 2002-2-29 In all-out war warriors get x2 below 80 and x1.7 at 80~100; mages x1.7 below 80 and x1.5 at 80~100
v2.15 2002-2-29 Raised base all-out war experience and adjusted experience for killing enemies.
v2.15 2002-2-28 On farmland level 80+ gets only 2/3 experience
v2.15 2002-2-26 On farmland level 100+ gets only half experience
v2.15 2002-2-26 Changed the supreme item upgrade routine
v2.14 2002-2-18 Added GM logs for summon, poison, skill, item repair, Give, and money
v2.14 2002-2-18 Whispers to GMs are also written to the GM log
v2.14 2002-2-18 Translated Skill.cfg to Chinese for the skill log
v2.14 2002-2-17 Added a routine that restores a failed GM teleport
v2.14 2002-2-16 Fixed odd purity when storing items in the warehouse (client change needed)
v2.14 2002-2-15 Faster world server saves (sends and receives CharID) DEF_DBGAMESERVER
v2.14 2002-2-15 Lowered the demon spawn chance on farmland
v2.14 2002-2-15 Lowered the chance of duplicate unique numbers DEF_LOGTIME
v2.14 2002-2-15 Set the GM Level for GM commands as Taiwan asked
v2.14 2002-2-15 GMs take no damage from meteors
v2.14 2002-2-15 Fixed debugging when built as a game server library !! See MSDN, compile with /C7
v2.14 2002-2-14 Teleport into the resurrection zone allowed
v2.14 2002-2-14 Fixed the bug where neutrals could not recall
v2.14 2002-2-13 Found and fixed a log bug: added pItem = NULL
v2.14 2002-2-13 Strengthened unique numbers (month day hour): added the hour
v2.14 2002-2-13 Added exception handling to a routine suspected of crashing the game server (def_itemlog_buy)
v2.14 2002-2-12 Log even chat messages for GMs
v2.14 2002-2-12 Added /fi with coordinates (client changed)
v2.14 2002-2-12 No item discount at Charisma 10
v2.14 2002-2-12 Unique numbers for shop items too
v2.14 2002-2-12 Found and fixed broken GM item logs
v2.14 2002-2-11 Declaration for Chinese NPC names DEF_LOCALNPCNAME (client changed)
v2.14 2002-2-11 Strengthened and standardised the item log: added the new _bItemLog, and buying several of the same item in a shop logs once
v2.14 2002-2-11 Fixed a Chinese translation bug with guilds (DEF_ENGLISHITEMNAME)
v2.14 2002-2-11 GM Log sent to the world server into one file (world server changed)
v2.14 2002-2-10 Strengthened the item log: map coordinates, the NPC that dropped the item, etc.
v2.14 2002-2- 9 Fixed the server crash on /disconnect
*/





