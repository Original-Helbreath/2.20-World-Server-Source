// Npc.h: interface for the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPC_H__6F7A5C80_A709_11D2_B143_00001C7030A6__INCLUDED_)
#define AFX_NPC_H__6F7A5C80_A709_11D2_B143_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <windows.h>
#include <winbase.h>
#include "Magic.h"
#include <vector>

#define DEF_MAXWAYPOINTS			10

#define DEF_MOVETYPE_STOP			0		// Stands still. The default behaviour of an initialised NPC.
#define DEF_MOVETYPE_SEQWAYPOINT	1		// Follows the path in order. Keeps doing that action at the last point.
#define DEF_MOVETYPE_RANDOMWAYPOINT	2		// Picks path points at random.
#define DEF_MOVETYPE_FOLLOW			3		// Follows a given target.
#define DEF_MOVETYPE_RANDOMAREA		4		// Moves to random places inside a given area.
#define DEF_MOVETYPE_RANDOM			5		// Moves anywhere on the map.
#define DEF_MOVETYPE_GUARD			6		// Stays where it spawned but chases a target when found.

#define DEF_BEHAVIOR_STOP			0
#define DEF_BEHAVIOR_MOVE			1
#define DEF_BEHAVIOR_ATTACK			2
#define DEF_BEHAVIOR_FLEE			3
#define DEF_BEHAVIOR_DEAD			4

#define DEF_ATTRIBUTE_EARTH			1
#define DEF_ATTRIBUTE_AIR			2
#define DEF_ATTRIBUTE_FIRE			3
#define DEF_ATTRIBUTE_WATER			4

// 2002-09-06 #1
class CNpcItem
{
 public :
	CNpcItem() ;

	char  m_cName[21];
	short m_sItemID;				// 2002-09-09 #1 Item ID
	short m_sFirstProbability;
	char  m_cFirstTargetValue;
	short m_sSecondProbability;
	char  m_cSecondTargetValue;
};


class CNpc  
{
public:

	CNpc(char * pName5);
	virtual ~CNpc();

	char  m_cNpcName[21]; // NPC name. Like a person's name
	
	char  m_cName[6];	// This name is an identifier for the program
	char  m_cMapIndex;
	short m_sX, m_sY;
	short m_dX, m_dY;
	short m_vX, m_vY;
	int   m_tmp_iError;
	RECT  m_rcRandomArea;	// Area coordinates for MOVETYPE_RANDOMAREA

	char  m_cDir;
	char  m_cAction;
	char  m_cTurn;

	short m_sType;
	short m_sOriginalType;
	short m_sAppr2;
	short m_sStatus;

	DWORD m_dwTime;
	DWORD m_dwActionTime;
	DWORD m_dwHPupTime, m_dwMPupTime;
	DWORD m_dwDeadTime, m_dwRegenTime;

	int  m_iHP;						// Hit Point 
	int  m_iExp;                    // Experience for a kill. Decided by ExpDice.

	int  m_iHitDice;				// Monster Hit Dice. HP is decided by it.
	int  m_iDefenseRatio;			// Defense Ratio
	int  m_iHitRatio;				// HitRatio
	int  m_iMagicHitRatio;			// v1.4 Magic attack Ratio
	int  m_iMinBravery;			    // Minimum bravery
	int  m_iExpDice;				// Experience dice on death X dice 4
		
	char m_cSide;					// The NPC's team
	char m_cActionLimit;            // 1 means it can only Move and never dies.  2 means it neither moves nor dies. 3 is a Dummy, for being hit,
	                                // 4 is an energy sphere, 5 a building

	char m_cSize;					// 0: Small-Medium 1: Large
	char m_cAttackDiceThrow;		// Number of attack dice thrown
	char m_cAttackDiceRange;		// Attack dice range
	char m_cAttackBonus;			// Attack bonus
	char m_cBravery;				// NPC bravery
	char m_cResistMagic;			// Magic resistance
	char m_cMagicLevel;				// Magic level
	char m_cDayOfWeekLimit;		    // Spawn date limit
	char m_cChatMsgPresence;		// Whether a linked Chat Msg exists
	int  m_iMana;                   // Maximum is MagicLevel*30
	int  m_iMaxMana;				// v1.4 New additions.
																    
	char  m_cMoveType;				// NPC movement path type
	char  m_cBehavior;				// NPC behaviour
	short m_sBehaviorTurnCount;		// Behaviour count
	char  m_cTargetSearchRange;		// NPC attack target search range

	int   m_iFollowOwnerIndex;		// Index of the object the NPC follows
	char  m_cFollowOwnerType;		// Type of the followed object (NPC or Player)
	BOOL  m_bIsSummoned;            // A summoned NPC does not regain HP.
	DWORD m_dwSummonedTime;			// Summoned mob creation time

	int   m_iTargetIndex;			// NPC attack target index
	char  m_cTargetType;			// Attack target type (NPC or Player)
	char  m_cCurWaypoint;			// Current move position
	char  m_cTotalWaypoint;			// Total move positions

	int   m_iSpotMobIndex;			// Stores the index if created by a spot-mob-generator
	int   m_iWayPointIndex[DEF_MAXWAYPOINTS+1];
	char  m_cMagicEffectStatus[DEF_MAXMAGICEFFECTS];

	BOOL  m_bIsPermAttackMode;		// With this flag set it does not change target mid-attack.
   	int   m_iNoDieRemainExp;		// Experience gained per hit without killing
	int   m_iAttackStrategy; 		// Combat doctrine AI type
	int   m_iAILevel;				// Intelligence level

	int   m_iAttackRange;			// Attack range; 1 or more means ranged attacks are possible
	/*
		AI-Level 
			1: normal behaviour
			2: chase the target with the lowest energy
			3: exclude magic-protected targets from magic attacks
	*/
	int   m_iAttackCount;			// Combo attack motion count
	BOOL  m_bIsKilled;


	int   m_iLastDamage;			// Damage taken before the final death. Too much damage means no alchemy ingredient drops.
	int   m_iSummonControlMode;		// Summoned mob control mode: 0 Free, 1 Hold, 2 Tgt
	char  m_cAttribute;				// Magic attribute:  earth 1 air 2 fire 3 water 4
	int   m_iAbsDamage;				// Positive is the magic damage reduction rate, negative the physical damage reduction rate

	int   m_iItemRatio;				// Special item chance
	int   m_iAssignedItem;			// Assigned item

	char  m_cSpecialAbility;		// Special ability 0: none

									/*
case 0: break;
case 1:  "Penetrating Invisibility"
case 2:  "Breaking Magic Protection"
case 3:  "Absorbing Physical Damage"
case 4:  "Absorbing Magical Damage"
case 5:  "Poisonous"
case 6:  "Extremely Poisonous"
case 7:  "Explosive"
case 8:  "Hi-Explosive" 

 A value above 60 is unrelated to NPC effects and is ignored.
									*/

	int	  m_iBuildCount;			// For structures such as guard towers, the pickaxe hits left until complete. Starts at m_iMinBravery.
									// Acts only when this is 0.
	int   m_iManaStock;				// For the Grand Magic Generator, the stored mana
	BOOL  m_bIsMaster;				// Whether it is a master mob
	int   m_iGuildGUID;				// For war units, the summoner's guild ID.

	int   m_iV1;

	// 2002-09-06 #1
	std::vector<CNpcItem>		m_vNpcItem;

	// 2002-09-17 #1
	int		m_iNpcitemType;		// 1 : normal   (may drop no item.)
								// 2 : special monster (Tigerworm, Hellclaw, etc.)

	int		m_iNpcitemMax;		// Max dice value when npcitem type is 2

	int		m_iNpcCrops;		//v2.19 2002-12-16 Farming skill :: crop number :: 0 means not a crop.
	int		m_iCropsSkillLV;		//v2.19 2002-12-16 Farming skill :: crop level
};

#endif // !defined(AFX_NPC_H__6F7A5C80_A709_11D2_B143_00001C7030A6__INCLUDED_)
