// Client.cpp: implementation of the CClient class.
//
//////////////////////////////////////////////////////////////////////

#include "Client.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClient::CClient(HWND hWnd)
{
 register int i;
	
	m_pXSock = NULL;
	m_pXSock = new class XSocket(hWnd, DEF_CLIENTSOCKETBLOCKLIMIT);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);

	ZeroMemory(m_cProfile, sizeof(m_cProfile));
	strcpy(m_cProfile, "__________");

	ZeroMemory(m_cCharName, sizeof(m_cCharName));
	ZeroMemory(m_cAccountName, sizeof(m_cAccountName));
	ZeroMemory(m_cAccountPassword, sizeof(m_cAccountPassword));

	ZeroMemory(m_cGuildName, sizeof(m_cGuildName));
	ZeroMemory(m_cLocation, sizeof(m_cLocation));
	ZeroMemory(m_cMapName, sizeof(m_cMapName));
	
	strcpy(m_cLocation, "NONE");
	m_iGuildRank = -1;
	m_iGuildGUID = -1;

	m_bIsInitComplete = FALSE;

	m_cLU_Str = m_cLU_Int = m_cLU_Vit = m_cLU_Dex = m_cLU_Mag = m_cLU_Char = 0;

	// v1.432 Not used.
	//m_iHitRatio_ItemEffect_SM = 0;
	//m_iHitRatio_ItemEffect_L  = 0;

	m_iEnemyKillCount = 0;
	m_iPKCount = 0;
	m_iRewardGold = 0;
	m_iCurWeightLoad = 0;

	m_bIsSafeAttackMode  = FALSE;

	// Reset the item equip state, then set it.
	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) 
		m_sItemEquipmentStatus[i] = -1;
	
	// Reset the item list
	for (i = 0; i < DEF_MAXITEMS; i++) {
		m_pItemList[i]       = NULL;
		m_ItemPosList[i].x   = 40;
		m_ItemPosList[i].y   = 30;
		m_bIsItemEquipped[i] = FALSE;
	}
	m_cArrowIndex = -1;	// Arrow item index not yet assigned

	// Reset the banked item list.
	for (i = 0; i < DEF_MAXBANKITEMS; i++) {
		m_pItemInBankList[i] = NULL;
	}

	// Reset the Magic - Skill mastery list
	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		m_cMagicMastery[i] = NULL;
	
	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		m_cSkillMastery[i] = NULL;

	for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
		m_bSkillUsingStatus[i] = FALSE;
		m_iSkillUsingTimeID[i] = NULL;
	}

	// testcode
	m_cMapIndex = -1;
	m_sX = -1;
	m_sY = -1;
	m_cDir = 5;
	m_sType   = 0;
	m_sOriginalType = 0;
	m_sAppr1  = 0;
	m_sAppr2  = 0;
	m_sAppr3  = 0;
	m_sAppr4  = 0;
	m_iApprColor = 0; // v1.4
	m_sStatus = 0;

	m_cSex  = 0;
	m_cSkin = 0;
	m_cHairStyle  = 0;
	m_cHairColor  = 0;
	m_cUnderwear  = 0;

	m_cAttackDiceThrow_SM = 0;	// Number of attack dice thrown @@@@@@@@@@@@@
	m_cAttackDiceRange_SM = 0;
	m_cAttackDiceThrow_L = 0;	// Number of attack dice thrown @@@@@@@@@@@@@
	m_cAttackDiceRange_L = 0;
	m_cAttackBonus_SM    = 0;
	m_cAttackBonus_L     = 0;
	
	// The side follows the player's home town; NPCs use it to decide whether to attack.
	m_cSide = 0;

	m_iHitRatio = 0;
	m_iDefenseRatio = 0;
	
	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) m_iDamageAbsorption_Armor[i]  = 0;
	m_iDamageAbsorption_Shield = 0;

	// v2.20 2002-12-28 Fixed the 3rd anniversary ring bug
	m_iHPstock = 0;
	m_iHPStatic_stock = 0 ;

	m_bIsKilled = FALSE;

	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++) 
		m_cMagicEffectStatus[i]	= 0;

	m_iWhisperPlayerIndex = -1;
	ZeroMemory(m_cWhisperPlayerName, sizeof(m_cWhisperPlayerName));
	
	m_iHungerStatus  = 100;  // Maximum value is 100
	
	m_bIsWarLocation = FALSE;
	
	m_bIsPoisoned    = FALSE;
	m_iPoisonLevel   = NULL;

	m_iAdminUserLevel  = 0;
	m_iRating          = 0;
	m_iTimeLeft_ShutUp = 0;
	m_iTimeLeft_Rating = 0;
	m_iTimeLeft_ForceRecall  = 0;
	m_iTimeLeft_FirmStaminar = 0;

	m_bIsOnServerChange  = FALSE;

	m_iExpStock = 0;

	m_iAllocatedFish = NULL;
	m_iFishChance    = 0;

	ZeroMemory(m_cIPaddress, sizeof(m_cIPaddress)); 
	m_bIsOnWaitingProcess = FALSE;

	m_iSuperAttackLeft  = 0;
	m_iSuperAttackCount = 0;

	m_sUsingWeaponSkill = 5; // Bare-handed fighting by default

	m_iManaSaveRatio   = 0;
	m_iAddResistMagic  = 0;
	m_iAddPhysicalDamage = 0;
	m_iAddMagicalDamage  = 0;
	m_bIsLuckyEffect     = FALSE;
	m_iSideEffect_MaxHPdown = 0;

	m_iAddAbsAir   = 0;	// Damage absorption per attribute
	m_iAddAbsEarth = 0;
	m_iAddAbsFire  = 0;
	m_iAddAbsWater = 0;

	m_iComboAttackCount = 0;
	m_iDownSkillIndex   = -1;

	m_iMagicDamageSaveItemIndex = -1;

	m_sCharIDnum1 = m_sCharIDnum2 = m_sCharIDnum3 = 0;
	
	m_iPartyRank = -1; // v1.42
	m_iPartyMemberCount = 0;
	m_iPartyGUID        = 0;

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}

	m_iAbuseCount     = 0;
	m_bIsBWMonitor    = FALSE;
	m_bIsExchangeMode = FALSE;

	// v1.4311-3 Reset the added fight-zone reservation variables
    m_iFightZoneTicketNumber =	m_iFightzoneNumber = m_iReserveTime = 0 ;            

	m_iPenaltyBlockYear = m_iPenaltyBlockMonth = m_iPenaltyBlockDay = 0; // v1.4

	m_iExchangeH = NULL;											// Index of the trade partner
	ZeroMemory(m_cExchangeName, sizeof(m_cExchangeName));			// Name of the trade partner
	ZeroMemory(m_cExchangeItemName, sizeof(m_cExchangeItemName));	// Name of the item to trade

	m_cExchangeItemIndex  = -1; // Index of the item to trade
	m_iExchangeItemAmount = 0;  // Count of the item to trade

	m_bIsExchangeConfirm = FALSE;

	m_iQuest		 = NULL; // Currently assigned quest
	m_iQuestID       = NULL; // QuestID
	m_iAskedQuest	 = NULL; // Quest asked about
	m_iCurQuestCount = NULL; // Current quest state

	m_iQuestRewardType	 = NULL; // Reward type on quest completion -> the item ID.
	m_iQuestRewardAmount = NULL; // Reward count

	m_iContribution = NULL;			// Contribution
	m_bQuestMatchFlag_Loc = FALSE;  // Flag for checking the quest location.
	m_bIsQuestCompleted   = FALSE;

	m_bIsNeutral      = FALSE;
	m_bIsObserverMode = FALSE;

	// 2000.8.1 for checking the event reward was given
	m_iSpecialEventID = 200081;

	m_iSpecialWeaponEffectType  = 0;	// Rare item effect type: 0-None 1-extra critical damage 2-poison 3-righteous 4-cursed
	m_iSpecialWeaponEffectValue = 0;	// Rare item effect value

	m_iAddHP = m_iAddSP = m_iAddMP = 0; 
	m_iAddAR = m_iAddPR = m_iAddDR = 0;
	m_iAddAbsPD = m_iAddAbsMD = 0;
	m_iAddCD = m_iAddExp = m_iAddGold = 0;
		
	m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;		// The special ability can be used once every DEF_SPECABLTYTIMESEC seconds.
	m_iSpecialAbilityType = NULL;
	m_bIsSpecialAbilityEnabled = FALSE;
	m_iSpecialAbilityLastSec   = 0;

	m_iSpecialAbilityEquipPos  = 0;

	m_iMoveMsgRecvCount   = 0;
	m_iAttackMsgRecvCount = 0;
	m_iRunMsgRecvCount    = 0;
	m_iSkillMsgRecvCount  = 0;

	m_bIsAdminCreateItemEnabled = FALSE;
	m_bIsAdminCommandEnabled = FALSE;   // v2.18 2002-10-15 Added a password to important GM commands
	m_iAlterItemDropIndex = -1;

	m_iAutoExpAmount = 0;
	m_iWarContribution = 0;

	m_dwMoveLAT = m_dwRunLAT = m_dwAttackLAT = 0;

	m_dwInitCCTimeRcv = 0;
	m_dwInitCCTime = 0;

	ZeroMemory(m_cLockedMapName, sizeof(m_cLockedMapName));
	strcpy(m_cLockedMapName, "NONE");
	m_iLockedMapTime   = NULL;
	m_iDeadPenaltyTime = NULL;

	m_iCrusadeDuty  = NULL;
	m_dwCrusadeGUID = NULL;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		m_stCrusadeStructureInfo[i].cType = NULL;
		m_stCrusadeStructureInfo[i].cSide = NULL;
		m_stCrusadeStructureInfo[i].sX = NULL;
		m_stCrusadeStructureInfo[i].sY = NULL;
	}
	m_iCSIsendPoint = NULL;

	m_bIsSendingMapStatus = FALSE;
	ZeroMemory(m_cSendingMapName, sizeof(m_cSendingMapName));

	m_iConstructionPoint = NULL;

	ZeroMemory(m_cConstructMapName, sizeof(m_cConstructMapName));
	m_iConstructLocX = m_iConstructLocY = -1;

	m_dwFightzoneDeadTime = NULL;

	m_iPartyID = NULL;
	m_iPartyStatus = DEF_PARTYSTATUS_NULL;

	m_iReqJoinPartyClientH = NULL;
	ZeroMemory(m_cReqJoinPartyName, sizeof(m_cReqJoinPartyName));

	m_dwLastActionTime = NULL;
	m_bIsCheckingWhisperPlayer = FALSE;

	// v2.13 Added to reduce DB load
	m_bIsBankModified = FALSE ;

	m_iGizonItemUpgradeLeft = 0;

	m_dwAttackFreqTime = m_dwMagicFreqTime = m_dwMoveFreqTime = NULL; // v2.171
	m_bIsMoveBlocked = FALSE; // v2.171
	m_bIsAttackModeChange = FALSE; // v2.172 2002-7-2
	m_iIsOnTown = FALSE;
	m_bIsOnShop = FALSE ; // v2.182 2002-11-15 Added a variable that checks whether this is a place to buy and sell
	m_bIsOnTower = FALSE ; // v2.20 2002-12-23 Checks whether magic can be learned here.
	m_bIsOnWarehouse = FALSE ; // v2.20 2002-12-23 Checks whether this is a warehouse.
	m_bIsHunter = FALSE ; // v2.182 2002-11-15 Added a variable that checks whether the player is a hunter
	m_dwWarmEffectTime = NULL; // v2.172 
	m_bIsInBuilding = FALSE ;
}

CClient::~CClient()
{
 int i;
	
	if (m_pXSock != NULL) delete m_pXSock;
	for (i = 0; i < DEF_MAXITEMS; i++)
	if (m_pItemList[i] != NULL) {
		delete m_pItemList[i];
		m_pItemList[i] = NULL;
	}
}


BOOL CClient::bCreateNewParty()
{
 int i;

	if (m_iPartyRank != -1) return FALSE;

	m_iPartyRank = 0;
	m_iPartyMemberCount = 0;
	m_iPartyGUID = (rand() % 999999) + timeGetTime();

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}

	return TRUE;
}
