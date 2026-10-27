// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include "XSocket.h"
#include "Item.h"
#include "GuildsMan.h"
#include "Magic.h"
#include "GlobalDef.h"

#define DEF_CLIENTSOCKETBLOCKLIMIT	15

#define DEF_MSGBUFFERSIZE	30000
#define DEF_MAXITEMS		50
#define DEF_MAXBANKITEMS	120
#define DEF_MAXGUILDSMAN	128 // Maximum guild members

#define	DEF_MAXMAGICTYPE	100	// Changing this requires changing the log server too.
#define DEF_MAXSKILLTYPE	60

// 2002-12-6  Added the Teleport feature
#define DEF_MAXTELEPORTLIST	20

#define DEF_MAXPARTYMEMBERS	8		// 2002-09-05 #3 adamas 6 -> 8 not sure why this was 6.

#define DEF_SPECABLTYTIMESEC	1200

#define DEF_PARTYSTATUS_NULL		0
#define DEF_PARTYSTATUS_PROCESSING	1
#define DEF_PARTYSTATUS_CONFIRM		2

class CClient  
{
public:

	BOOL bCreateNewParty();
	
	CClient(HWND hWnd);
	virtual ~CClient();

	char m_cCharName[12];
	char m_cAccountName[12];
	char m_cAccountPassword[12];

	BOOL  m_bIsInitComplete;
	BOOL  m_bIsMsgSendAvailable;
	BOOL  m_bIsCheckingWhisperPlayer;

	char  m_cMapName[12];
	char  m_cMapIndex;
	short m_sX, m_sY;
	
	char  m_cGuildName[22];		// Guild name
	char  m_cLocation[12];      //  <- stored as the map name. Home town
	int   m_iGuildRank;			// -1 means none. 0 is guild master. Positive is the rank within the guild
	int   m_iGuildGUID;
	
	char  m_cDir;
	short m_sType;				// Current type <- changes on polymorph.
	short m_sOriginalType;		// Original type
	short m_sAppr1;
	short m_sAppr2;
	short m_sAppr3;
	short m_sAppr4;             // 
	int   m_iApprColor;			// v1.4 Appearance colour table
	short m_sStatus;

	DWORD m_dwTime, m_dwHPTime, m_dwMPTime, m_dwSPTime, m_dwAutoSaveTime, m_dwHungerTime;

	// Player stats

	char m_cSex, m_cSkin, m_cHairStyle, m_cHairColor, m_cUnderwear;

	int  m_iHP;						// Hit Point
	int  m_iHPstock;
	int  m_iHPStatic_stock;         // v2.20 2002-12-28 Fixed the 3rd anniversary ring bug
	int  m_iMP;
	int  m_iSP;
	int  m_iExp, m_iNextLevelExp;
	BOOL m_bIsKilled;

	int  m_iDefenseRatio;		// Defense Ratio
	int  m_iHitRatio;			// Hit Ratio

	// v1.432 Not used.
	//int  m_iHitRatio_ItemEffect_SM; // HitRatio change from equipped items
	//int  m_iHitRatio_ItemEffect_L;

	int  m_iDamageAbsorption_Armor[DEF_MAXITEMEQUIPPOS];		// Damage absorption from worn armour
	int  m_iDamageAbsorption_Shield;	// Damage absorption on a successful Parrying

	int  m_iLevel;
	int  m_iStr, m_iInt, m_iVit, m_iDex, m_iMag, m_iCharisma;
	char m_cLU_Str, m_cLU_Int, m_cLU_Vit, m_cLU_Dex, m_cLU_Mag, m_cLU_Char;   // Stat points granted on level-up.
	int  m_iLuck; 

	int  m_iEnemyKillCount, m_iPKCount, m_iRewardGold;
	int  m_iCurWeightLoad;		// Current total carried weight

	char m_cSide;				// Player's side

	char m_cAttackDiceThrow_SM;	// Number of attack dice thrown
	char m_cAttackDiceRange_SM;	// Attack dice range
	char m_cAttackDiceThrow_L;	// Number of attack dice thrown
	char m_cAttackDiceRange_L;	// Attack dice range
	char m_cAttackBonus_SM;		// Attack bonus
	char m_cAttackBonus_L;		// Attack bonus

	class CItem * m_pItemList[DEF_MAXITEMS];
	POINT m_ItemPosList[DEF_MAXITEMS];
	class CItem * m_pItemInBankList[DEF_MAXBANKITEMS];
	
	BOOL  m_bIsItemEquipped[DEF_MAXITEMS];
	short m_sItemEquipmentStatus[DEF_MAXITEMEQUIPPOS];
	char  m_cArrowIndex;		// Arrow item index used when the player shoots a bow. Initial value -1 (not assigned)

	char           m_cMagicMastery[DEF_MAXMAGICTYPE];
	unsigned char  m_cSkillMastery[DEF_MAXSKILLTYPE]; // v1.4

	int   m_iSkillSSN[DEF_MAXSKILLTYPE];
	BOOL  m_bSkillUsingStatus[DEF_MAXSKILLTYPE];
	int   m_iSkillUsingTimeID[DEF_MAXSKILLTYPE]; //v1.12

	char  m_cMagicEffectStatus[DEF_MAXMAGICEFFECTS];

	int   m_iWhisperPlayerIndex;
	char  m_cWhisperPlayerName[12];
	char  m_cProfile[256];

	int   m_iHungerStatus;		// Hunger points. At 0 stamina stops regenerating and HP fills no more than half.

	DWORD m_dwWarBeginTime;		// Time recorded on entering an enemy country.
	BOOL  m_bIsWarLocation;		// Whether currently in an enemy country

	BOOL  m_bIsPoisoned;		// Whether poisoned
	int   m_iPoisonLevel;       // Poison strength
	DWORD m_dwPoisonTime;		// Poison time.
	
	int   m_iPenaltyBlockYear, m_iPenaltyBlockMonth, m_iPenaltyBlockDay; // v1.4

	//v1.4311-3 Added fight-zone number and fight-zone reservation time
	int   m_iFightzoneNumber , m_iReserveTime, m_iFightZoneTicketNumber ; 

	class XSocket * m_pXSock;

	int   m_iAdminUserLevel;     // Admin level. 0 is none. Higher numbers have more rights.
	int   m_iRating;

	int   m_iTimeLeft_ShutUp;	 // If this is not 0, chat messages are not delivered.
	int   m_iTimeLeft_Rating;	 // Time left before rating another player. 0 means available
	int   m_iTimeLeft_ForceRecall;  // Time ticks left before a forced recall
	int   m_iTimeLeft_FirmStaminar; // Time span during which stamina does not drain

	BOOL  m_bIsOnServerChange;     // If set, data is not saved or counted on delete.

	int   m_iExpStock;			 // Stocked experience
	DWORD m_dwExpStockTime;		 // ExpStock calculation time.

	int   m_iAutoExpAmount;		 // Experience gained during the Auto-Exp period
	DWORD m_dwAutoExpTime;		 // Auto-Exp calculation time.

	DWORD m_dwRecentAttackTime;  // Time of the most recent attack
	DWORD m_dwLastActionTime;	 // Time of the most recent action: chat/move/attack/magic/skill all count

	int   m_iAllocatedFish;		 // If this is not 0, event fishing mode is on.
	int   m_iFishChance;		 // Current fishing state
	
	char  m_cIPaddress[21];		 // Client IP address
	BOOL  m_bIsSafeAttackMode;

	BOOL  m_bIsOnWaitingProcess; // If waiting for processing such as a teleport
	
	int   m_iSuperAttackLeft;	 // v1.2 Critical attacks available
	int   m_iSuperAttackCount;   // v1.2 Critical attack counter. When it fills, the critical attack count rises.

	short m_sUsingWeaponSkill;	 // v1.2 Skill index of the weapon in use

	int   m_iManaSaveRatio;		 // v1.2 Mana saving points
	
	BOOL  m_bIsLuckyEffect;		 // v1.2 Luck effect
	int   m_iSideEffect_MaxHPdown; // v1.4 Max HP reduction effect

	int   m_iComboAttackCount;   // v1.3 Combo attack count
	int   m_iDownSkillIndex;	 // v1.3 Skill index to lower when another skill rises

	int   m_iMagicDamageSaveItemIndex; // v1.3 Index of the magic damage reduction item (normally -1)

	short m_sCharIDnum1, m_sCharIDnum2, m_sCharIDnum3; // v1.3 The character's unique value!

	// v2.06 12-2 This variable is not used.
	int   m_iPartyRank;										// Position in the party. -1 means none. 1 is the party creator. 12 is a member
	int   m_iPartyMemberCount;								// For the party size limit
	int   m_iPartyGUID;										// v1.42 Party GUID
	struct {
		int  iIndex;
		char cName[11];

	} m_stPartyMemberName[DEF_MAXPARTYMEMBERS];
	//

	int   m_iAbuseCount;		// For identifying hacking suspects
	
	BOOL  m_bIsBWMonitor;		// Is this a BadWord monitor?
	// v2.15 2002-5-6
	BOOL  m_bIsManager;		    // Is this the Manager program?


	BOOL  m_bIsExchangeMode;		// Currently in item trade mode?
	int   m_iExchangeH;				// Index of the trade partner
	char  m_cExchangeName[11];		// Name of the trade partner
	char  m_cExchangeItemName[21];	// Name of the item to trade

	char  m_cExchangeItemIndex;  // Index of the item to trade
	int   m_iExchangeItemAmount; // Count of the item to trade

	BOOL  m_bIsExchangeConfirm;  // Trade confirmed

	int   m_iQuest;				 // Currently assigned quest
	int   m_iQuestID;			 // ID of the assigned quest
	int   m_iAskedQuest;		 // Quest asked about
	int   m_iCurQuestCount;		 // Current quest state
	
	int   m_iQuestRewardType;	 // Reward type on quest completion -> the item ID.
	int   m_iQuestRewardAmount;	 // Reward count

	int   m_iContribution;		 // Contribution to the city.

	BOOL  m_bQuestMatchFlag_Loc; // For judging the quest conditions.
	BOOL  m_bIsQuestCompleted;   // Is the quest completed?

	int   m_iCustomItemValue_Attack;
	int   m_iCustomItemValue_Defense;

	int   m_iMinAP_SM;			// Minimum AP from Custom-Item effects
	int   m_iMinAP_L;

	int   m_iMaxAP_SM;			// Maximum AP from Custom-Item effects
	int   m_iMaxAP_L;

	BOOL  m_bIsNeutral;			// v1.5 Flag marking neutral players, to speed up processing.
	BOOL  m_bIsObserverMode;	// v1.5 Whether in observer mode

	int   m_iSpecialEventID;	// Flag for special event participation

	int   m_iSpecialWeaponEffectType;	// Rare item effect type
	int   m_iSpecialWeaponEffectValue;	// Rare item effect value
	// Rare item effect type:
	// 0-None 1-extra critical damage 2-poison 3-righteous
	// 5-agile 6-light 7-sharp 8-strengthened 9-ancient

	// v1.42
	int   m_iAddHP, m_iAddSP, m_iAddMP; 
	int   m_iAddAR, m_iAddPR, m_iAddDR;
	int   m_iAddMR, m_iAddAbsPD, m_iAddAbsMD; 
	int   m_iAddCD, m_iAddExp, m_iAddGold;		// Fixed magic damage absorption. Calculated independently of rings.

	int   m_iAddTransMana, m_iAddChargeCritical;

	int   m_iAddResistMagic;					// v1.2 Extra magic resistance
	int   m_iAddPhysicalDamage;					// v1.2 Fixed extra damage points
	int   m_iAddMagicalDamage;	

	int   m_iAddAbsAir;							// Damage absorption per attribute
	int   m_iAddAbsEarth;
	int   m_iAddAbsFire;
	int   m_iAddAbsWater;
	
	int   m_iLastDamage;

	int   m_iMoveMsgRecvCount, m_iAttackMsgRecvCount, m_iRunMsgRecvCount, m_iSkillMsgRecvCount;
	DWORD m_dwMoveLAT, m_dwRunLAT, m_dwAttackLAT;

	int   m_iSpecialAbilityTime;				// This must reach 0 before the special ability can be used.
	BOOL  m_bIsSpecialAbilityEnabled;			// Is the special ability active?
	DWORD m_dwSpecialAbilityStartTime;			// Time the special ability started
	int   m_iSpecialAbilityLastSec;				// Special ability duration.

	int   m_iSpecialAbilityType;				// Assigned special ability type
												// Attack type
												// 0:no ability  1:enemy HP -50% on hit  2:freeze  3:paralyse  4:one shot one kill  5:gain HP equal to damage dealt
												// Defence type
												// 50: weapon lifespan to 0. 51:nullify damage to that part  52: nullify damage to all parts
	int   m_iSpecialAbilityEquipPos;			// For armour, the body part the special effect applies to.
	BOOL  m_bIsAdminCreateItemEnabled;
	BOOL  m_bIsAdminCommandEnabled;             // v2.18 2002-10-15 Added a password to important GM commands
	int   m_iAlterItemDropIndex;				// Index of the item dropped instead

	int   m_iWarContribution;					// War contribution

	DWORD m_dwSpeedHackCheckTime;				// Speed bug check routine
	int   m_iSpeedHackCheckExp;		
	
	DWORD m_dwInitCCTimeRcv;
	DWORD m_dwInitCCTime;

	char  m_cLockedMapName[12];					// Name of the map the player is locked in
	int   m_iLockedMapTime;						// If this is above 0, any teleport goes to the map above.
	int   m_iDeadPenaltyTime;					// v2.04 Penalty time after death. Dying again while it remains locks the player in the map.

	int   m_iCrusadeDuty;						// Crusade role: 1-soldier. 2-builder. 3-commander
	DWORD m_dwCrusadeGUID;						// Crusade GUID

	// This structure copies the map contents. It cannot be sent at once, so it is sent in several parts.
	struct {
		char cType;
		char cSide;
		short sX, sY;
	} m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
	int m_iCSIsendPoint;

	char m_cSendingMapName[12];
	BOOL m_bIsSendingMapStatus;

	// Points a commander can build with. For a normal player, the accumulated value of their actions.
	int  m_iConstructionPoint;

	char m_cConstructMapName[12];
	int  m_iConstructLocX, m_iConstructLocY;

	DWORD m_dwFightzoneDeadTime;

	// v2.13 Added to reduce DB load
	BOOL m_bIsBankModified ;

	// v2.15 Faster DB saving
	DWORD m_dwCharID ;

	int m_iPartyID;
	int m_iPartyStatus;
	int m_iReqJoinPartyClientH;
	char m_cReqJoinPartyName[12];

	int m_iGizonItemUpgradeLeft;

	DWORD m_dwAttackFreqTime, m_dwMagicFreqTime, m_dwMoveFreqTime; // v2.171
	BOOL m_bIsMoveBlocked; // v2.171
	BOOL m_bIsAttackModeChange; // v2.172
	int  m_iIsOnTown; 
	BOOL m_bIsOnShop; // v2.182 2002-11-15 Added a variable that checks whether this is a place to buy and sell
	BOOL m_bIsHunter; // v2.182 2002-11-15 Added a variable that checks whether the player is a hunter
	BOOL m_bIsOnTower; // v2.20 2002-12-23 Checks whether magic can be learned here.
	BOOL m_bIsOnWarehouse ; // v2.20 2002-12-23 Checks whether this is a warehouse.
	BOOL m_bIsInBuilding; // v2.19 2002-11-18 Added a variable that checks whether inside a building

	DWORD m_dwWarmEffectTime; // v2.172

};

#endif // !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)

/*

Item data save order:
  Name Count TouchEffectType, TEV1, TEV2, TEV3, Color SpecEffectV1, SEV2, SEV3, Life, Attribute
         
  ToucheEffectType: DEF_ITET_DATE, ITET_ID, ITET_UNIQUE_OWNER

  SpecEffectV1 : max lifespan for a Custom-Item
  SpecEffectV2 : completion for a Custom-Item

*/