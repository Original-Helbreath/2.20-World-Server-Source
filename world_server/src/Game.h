// Game.h: interface for the CGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAME_H__C3D29FC5_755B_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_GAME_H__C3D29FC5_755B_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <direct.h>

#include "StrTok.h"
#include "Xsocket.h"
#include "Client.h"
#include "Npc.h"
#include "Map.h"
#include "ActionID.h"
#include "UserMessages.h"
#include "NetMessages.h"
#include "MessageIndex.h"
#include "Misc.h"
#include "Msg.h"
#include "Magic.h"
#include "Skill.h"
#include "DynamicObject.h"
#include "DelayEvent.h"
#include "Version.h"
#include "Fish.h"
#include "DynamicObject.h"
#include "DynamicObjectID.h"
#include "Potion.h"
#include "Mineral.h"
#include "Quest.h"
#include "BuildItem.h"
#include "TeleportLoc.h"
#include "GlobalDef.h"
#include "englishitem.h"
#include "koreaitem.h"
#include "teleport.h"

#define DEF_SERVERSOCKETBLOCKLIMIT	300

#define DEF_MAXCLIENTS			2000
#define DEF_MAXNPCS				5000
#define DEF_MAXMAPS				100
#define DEF_MAXITEMTYPES		5000
#define DEF_MAXNPCTYPES			100
#define DEF_MAXBUILDITEMS		300
#define DEF_CLIENTTIMEOUT		1000*10		//(10 seconds)
#define DEF_AUTOSAVETIME		60000*30	// Auto-save interval 30 minutes
#define DEF_HPUPTIME			1000*15		// HP regeneration interval
#define DEF_MPUPTIME			1000*20		// MP regeneration interval
#define DEF_SPUPTIME			1000*10		// SP regeneration interval

#define DEF_HUNGERTIME			1000*60		// Hunger interval
#define DEF_POISONTIME			1000*12		// Poison effect interval
#define DEF_SUMMONTIME			60000*5		// Summoned mobs live 5 minutes
#define DEF_NOTICETIME			80000		// Notice send interval
#define DEF_PLANTTIME			60000*5		//v2.20 2002-12-20 Crop lifetime.

#define DEF_EXPSTOCKTIME		1000*10		// ExpStock calculation interval
#define DEF_MSGQUENESIZE		100000		// Message queue size 100,000
#define DEF_AUTOEXPTIME			1000*60*6	// Automatic experience interval
#define DEF_TOTALLEVELUPPOINT	3			// Total points granted on level-up


#define DEF_MAXDYNAMICOBJECTS	60000
#define DEF_MAXDELAYEVENTS		60000
#define DEF_GUILDSTARTRANK		12

#define DEF_SSN_LIMIT_MULTIPLY_VALUE	2	// SSN-limit multiplier

#define DEF_MAXNOTIFYMSGS		1000		// Max notice messages
#define DEF_MAXSKILLPOINTS		700			// Skill point total
#define DEF_NIGHTTIME			40

#define DEF_CHARPOINTLIMIT		200			// Maximum of each stat
#define DEF_RAGPROTECTIONTIME	7000		// Seconds after which lag protection applies
#define DEF_MAXREWARDGOLD		99999999	// Reward maximum

#define DEF_ATTACKAI_NORMAL				1	// Always attack
#define DEF_ATTACKAI_EXCHANGEATTACK		2	// Alternate attack - retreat
#define DEF_ATTACKAI_TWOBYONEATTACK		3	// 2-1 attack, retreat

#define DEF_MAXFISHS					200
#define DEF_MAXMINERALS					200
#define DEF_MAXENGAGINGFISH				30  // Max people fishing for one fish
#define DEF_MAXPORTIONTYPES				500 // Max potion definitions

#define DEF_MOBEVENTTIME				300000 // 5 minutes
#define DEF_MAXQUESTTYPE				200

#define DEF_MAXSUBLOGSOCK				10

#define DEF_ITEMLOG_GIVE				1
#define DEF_ITEMLOG_DROP				2
#define DEF_ITEMLOG_GET					3
#define DEF_ITEMLOG_DEPLETE				4
#define DEF_ITEMLOG_NEWGENDROP			5
#define DEF_ITEMLOG_DUPITEMID			6
#define DEF_ITEMLOG_BUY					7        // 12-22 added
#define DEF_ITEMLOG_SELL				8     
#define DEF_ITEMLOG_RETRIEVE			9
#define DEF_ITEMLOG_DEPOSIT				10
#define DEF_ITEMLOG_EXCHANGE			11
#define DEF_ITEMLOG_MAGICLEARN			12
#define DEF_ITEMLOG_MAKE				13
#define DEF_ITEMLOG_SUMMONMONSTER		14
#define DEF_ITEMLOG_POISONED			15
#define DEF_ITEMLOG_SKILLLEARN			16
#define DEF_ITEMLOG_REPAIR				17
#define DEF_ITEMLOG_JOINGUILD           18
#define DEF_ITEMLOG_BANGUILD            19	// Log missing in Korea
#define DEF_ITEMLOG_RESERVEFIGZONE      20	//  "
#define DEF_ITEMLOG_APPLY               21	//  "
#define DEF_ITEMLOG_SHUTUP              22	//  "
#define DEF_ITEMLOG_CLOSECONN			23	//  "
#define DEF_ITEMLOG_SPELLFIELD			24	//  "
#define DEF_ITEMLOG_CREATEGUILD			25	//  "
#define DEF_ITEMLOG_GUILDDISMISS		26	//  "
#define DEF_ITEMLOG_SUMMONPLAYER        27	//  "
#define DEF_ITEMLOG_CREATE				28	//  "
#define DEF_ITEMLOG_UPGRADEFAIL         29
#define DEF_ITEMLOG_UPGRADESUCCESS      30


// v2.15 All-out war log
#define DEF_CRUSADELOG_ENDCRUSADE       1
#define DEF_CRUSADELOG_STARTCRUSADE     2
#define DEF_CRUSADELOG_SELECTDUTY       3
#define DEF_CRUSADELOG_GETEXP           4


#define DEF_PKLOG_BYPLAYER				1
#define DEF_PKLOG_BYPK					2
#define DEF_PKLOG_BYENERMY				3
#define DEF_PKLOG_BYNPC					4
#define DEF_PKLOG_BYOTHER				5
#define DEF_PKLOG_REDUCECRIMINAL        6


#define DEF_MAXDUPITEMID				100

#define DEF_MAXGUILDS					1000 // Guilds that can be online at once
#define DEF_MAXONESERVERUSERS			800	 // Max users allowed on one server. Extra users go to the resurrection zone, Bleeding Isle or farmland.

#define DEF_MAXGATESERVERSTOCKMSGSIZE	30000


#ifdef DEF_TAIWAN    // Taiwan builds at most 7.
#define DEF_MAXCONSTRUCTNUM				7
#else
#define DEF_MAXCONSTRUCTNUM				10
#endif

#define DEF_MAXSCHEDULE					10


//v1.4311-3  Max number of fight zones
#define DEF_MAXFIGHTZONE 10 

//============================
// #define DEF_LEVELLIMIT		130				// Trial level limit!!!
#define DEF_LEVELLIMIT		20				// Trial level limit!!!	// adamas
//============================

//============================
#define DEF_MINIMUMHITRATIO 15				// Minimum hit chance  //v2.172 raised 10->15%
//============================		

//============================
#define DEF_MAXIMUMHITRATIO	99				// Maximum hit chance
//============================

//============================
#if defined(DEF_TESTSERVER)
	// #define DEF_PLAYERMAXLEVEL	130				// Max level: used for m_iPlayerMaxLevel when not set in Npc.cfg.
	#define DEF_PLAYERMAXLEVEL	180				// Max level: used for m_iPlayerMaxLevel when not set in Npc.cfg.
#else
	#define DEF_PLAYERMAXLEVEL	180				// Max level: used for m_iPlayerMaxLevel when not set in Npc.cfg.
	// #define DEF_PLAYERMAXLEVEL	140
#endif
//============================

//============================
#define DEF_GMGMANACONSUMEUNIT	15			// Grand Magic Generator mana absorption unit.
//============================

#define DEF_MAXCONSTRUCTIONPOINT 30000		// Max summon points

#define DEF_MAXWARCONTRIBUTION	 500000

#define DEF_MAXPARTYMEMBERS		8
#define DEF_MAXPARTYNUM			5000

#define DEF_MAXGIZONPOINT		37			// adamas : top supreme upgrade

// v2.181 2002-10-24 Fewer string compares for server speed.
#define DEF_NETURAL             0
#define DEF_ARESDEN             1
#define DEF_ELVINE              2
#define DEF_BOTHSIDE			100

//v2.19 2002-11-19 
#define DEF_PK					0
#define DEF_NONPK				1
#define DEF_NEVERNONPK			2  //v2.19 2002-11-19 Never PK in the beginner middleland

// 2002-12-24 Limit the NPCs (excluding buildings) that can be summoned in all-out war. (Taiwan request)
#ifdef DEF_TAIWAN
	#define DEF_MAX_CRUSADESUMMONMOB	5
#else
	#define DEF_MAX_CRUSADESUMMONMOB	500
#endif

// v2.20 2002-12-31 Added civilian mode level and area limits
#define DEF_LIMITHUNTERLEVEL 100

class CGame  
{
public:
	int  iSetSide(int iClientH); // 2002-11-15 Set the Client side, onTown and onShop info
	void RequestHuntmode(int iClientH); // 2002-11-14 Added hunter mode
	void SetNoHunterMode(int iClientH,BOOL bSendMSG = FALSE); 	// v2.20 2002-12-31 Added civilian mode level and area limits
	BOOL _bCrusadeLog(int iAction,int iClientH,int iData, char * cName);
	void SetForceRecallTime(int iClientH) ; // v2.17 2002-7-15 
	BOOL bCheckClientMoveFrequency(int iClientH, DWORD dwClientTime); // v2.171
	BOOL bCheckClientMagicFrequency(int iClientH, DWORD dwClientTime); // v2.171
	BOOL bCheckClientAttackFrequency(int iClientH, DWORD dwClientTime); // v2.171
	void RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex); // v2.171
	void ArmorLifeDecrement(int iClientH, int sTargetH, char cOwnerType, int iValue);
	BOOL bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH,BOOL bBonus = FALSE) ;
	void RequestItemUpgradeHandler(int iClientH, int iItemIndex);
	BOOL bSerchMaster(int iNpcH);
	void GetExp(int iClientH, int iExp, BOOL bIsAttackerOwn = FALSE);
	void PartyOperationResult_Dismiss(int iClientH, char *pName, int iResult, int iPartyID);
	void RequestAcceptJoinPartyHandler(int iClientH, int iResult);
	void RequestDeletePartyHandler(int iClientH);
	void PartyOperationResult_Info(int iClientH, char * pName, int iTotal, char * pNameList);
	void GetPartyInfoHandler(int iClientH);
	void RequestDismissPartyHandler(int iClientH);
	void PartyOperationResult_Join(int iClientH, char *pName, int iResult, int iPartyID);
	void RequestJoinPartyHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void PartyOperationResult_Delete(int iPartyID);
	void PartyOperationResult_Create(int iClientH, char * pName, int iResult, int iPartyID);
	void PartyOperationResultHandler(char * pData);
	void RequestCreatePartyHandler(int iClientH);
	BOOL bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex);
	void ResurrectPlayer(int iClientH);
	void AdminOrder_GetFightzoneTicket(int iClientH);
	void KillCrusadeObjects();

	BOOL bReadCrusadeScheduleConfigFile(char * pFn);
	void CrusadeWarStarter();
	BOOL bCopyItemContents(class CItem * pOriginal, class CItem * pCopy);

	int  iGetMapLocationSide(char * pMapName);

	void ManualEndCrusadeMode(int iWinnerSide);
	void ChatMsgHandlerGSM(int iMsgType, int iV1, char * pName, char * pData, DWORD dwMsgSize);
	BOOL bReadCrusadeGUIDFile(char * cFn);
	void _CreateCrusadeGUID(DWORD dwCrusadeGUID, int iWinnerSide);
	void RemoveClientShortCut(int iClientH);
	BOOL bAddClientShortCut(int iClientH);
	void RequestSetGuildConstructLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName);
	void GSM_SetGuildConstructLoc(int iGuildGUID, int dX, int dY, char * pMapName);
	void GSM_ConstructionPoint(int iGuildGUID, int iPoint);
	void CheckCommanderConstructionPoint(int iClientH);
	void GlobalStartCrusadeMode();
	void GSM_SetGuildTeleportLoc(int iGuildGUID, int dX, int dY, char * pMapName);
	void SyncMiddlelandMapInfo();
	void _GrandMagicLaunchMsgSend(int iType, char cAttackerSide);
	// v2.15 Changed to show building HP
	void GrandMagicResultHandler(char * cMapName, int iCrashedStructureNum, int iStructureDamageAmount, int iCasualities, int iActiveStructure, int iSTCount,char * pData);
	void CalcMeteorStrikeEffectHandler(int iMapIndex);
	void DoMeteorStrikeDamageHandler(int iMapIndex);
	void GSM_RequestFindCharacter(WORD wReqServerID, WORD wReqClientH, char * pName,char * cCharName);
	// v2.15 2002-5-21
	void GSM_RequestShutupPlayer(char * cName,WORD wReqServerID, WORD wReqClientH, WORD wV1,char * cTemp); 
	void ServerStockMsgHandler(char * pData);
	void SendStockMsgToGateServer();
	BOOL bStockMsgToGateServer(char * pData, DWORD dwSize);
	void RequestHelpHandler(int iClientH);
	void RemoveCrusadeStructures();

	// v2.20 2002-12-28 After the all-out war, force-recall civilians to town
	void RecallHunterPlayer();

	void _SendMapStatus(int iClientH);
	void MapStatusHandler(int iClientH, int iMode, char * pMapName);
	void SelectCrusadeDutyHandler(int iClientH, int iDuty);
	
	void CheckConnectionHandler(int iClientH, char *pData);
	void RequestSummonWarUnitHandler(int iClientH, int dX, int dY, char cType, char cNum, char cMode);
	void RequestGuildTeleportHandler(int iClientH);
	void RequestSetGuildTeleportLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName);
	void MeteorStrikeHandler(int iMapIndex);
	void _LinkStrikePointMapIndex();
	void MeteorStrikeMsgHandler(char cAttackerSide);
	void _NpcBehavior_GrandMagicGenerator(int iNpcH);
	void CollectedManaHandler(WORD wAresdenMana, WORD wElvineMana);
	void SendCollectedMana();
	void CreateCrusadeStructures();

	BOOL bReadCrusadeStructureConfigFile(char * cFn);
	void SaveOccupyFlagData();
	void LocalEndCrusadeMode(int iWinnerSide);
	void LocalStartCrusadeMode(DWORD dwGuildGUID);
	void CheckCrusadeResultCalculation(int iClientH);
	BOOL _bNpcBehavior_Detector(int iNpcH);
	BOOL _bNpcBehavior_ManaCollector(int iNpcH);
	BOOL __bSetConstructionKit(int iMapIndex, int dX, int dY, int iType, int iTimeCost, int iClientH);
	BOOL __bSetAgricultureItem(int iMapIndex, int dX, int dY, int iType, int iSsn,int iClientH);   //v2.19 2002-12-16 Farming skill
	BOOL bCropsItemDrop(int iClientH, short iTargetH,BOOL bMobDropPos = FALSE);												//v2.19 2002-12-16 Farming skill
	int bProbabilityTable(int x,int y,int iTable);												//v2.19 2002-12-16 Farming skill
	void AgingMapSectorInfo();
	void UpdateMapSectorInfo();

	BOOL bGetItemNameWhenDeleteNpc(int & iItemID, short sNpcType, int iItemprobability);

	int iGetItemWeight(class CItem * pItem, int iCount);
	void CancelQuestHandler(int iClientH);
	void ActivateSpecialAbilityHandler(int iClientH);
	void EnergySphereProcessor(BOOL bIsAdminCreate = FALSE, int iClientH = NULL);
	BOOL bCheckEnergySphereDestination(int iNpcH, short sAttackerH, char cAttackerType);
	void JoinPartyHandler(int iClientH, int iV1, char * pMemberName);
	void CreateNewPartyHandler(int iClientH);
	void _DeleteRandomOccupyFlag(int iMapIndex);
	void RequestSellItemListHandler(int iClientH, char * pData);
	void AdminOrder_EnableAdminCreateItem(int iClientH, char * pData, DWORD dwMsgSize);
	// v2.18 2002-10-15 Added a password to important GM commands
	void AdminOrder_EnableAdminCommand(int iClientH, char * pData, DWORD dwMsgSize); 
	void AdminOrder_CreateItem(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestRestartHandler(int iClientH);
	void AdminOrder_SetObserverMode(int iClientH);
	int iRequestPanningMapDataRequest(int iClientH, char * pData);
	void GetMagicAbilityHandler(int iClientH);
	void Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, BOOL bExp, int iAttr);
	void _TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY);
	void RequestCheckAccountPasswordHandler(char * pData, DWORD dwMsgSize);
	int _iTalkToNpcResult_Guard(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	void SetIceFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void _bDecodeNoticementFileContents(char * pData, DWORD dwMsgSize);
	void RequestNoticementHandler(int iClientH, char * pData);
	void _AdjustRareItemValue(class CItem * pItem);
	BOOL _bCheckDupItemID(class CItem * pItem);
	BOOL _bDecodeDupItemIDFileContents(char * pData, DWORD dwMsgSize);
	void NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType);
	// int  iGetPlayerABSStatus(int iWhatH, int iRecvH);
	int  iGetPlayerABSStatus(int iWhatH, int iRecvH); // 2002-12-2
	void AdminOrder_DisconnectAll(int iClientH, char * pData, DWORD dwMsgSize);
	void CheckSpecialEvent(int iClientH);
	void CheckSpecialEventThirdYear(int iClientH); // 2002-10-25 3rd anniversary event
	void AdminOrder_Summon(int iClientH, char * pData, DWORD dwMsgSize);
	// v2.14 Added the user summon command
	void AdminOrder_SummonPlayer(int iClientH, char * pData, DWORD dwMsgSize);
	// v2.15 2002-5-21
	BOOL _bDecodeWorldConfigFileContents(char * pData, DWORD dwMsgSize);
	// v2.17 2002-8-7 // 2002-09-06 #1
	BOOL _bDecodeNpcItemConfigFileContents(char * pData, DWORD dwMsgSize);
	// 2002-12-8 For World server security settings
	BOOL _bDecodeWLServerConfigFileContents(char * pData, DWORD dwMsgSize);
	

	char _cGetSpecialAbility(int iKindSA);
	void AdminOrder_UnsummonBoss(int iClientH);
	void AdminOrder_UnsummonAll(int iClientH);
	void AdminOrder_SetAttackMode(int iClientH, char * pData, DWORD dwMsgSize);
	// v2.17 2002-7-15 
	void AdminOrder_SetForceRecallTime(int iClientH, char * pData, DWORD dwMsgSize);
	
	void BuildItemHandler(int iClientH, char * pData);
	BOOL _bDecodeBuildItemConfigFileContents(char * pData, DWORD dwMsgSize);
	BOOL _bCheckSubLogSocketIndex();

	BOOL _bItemLog(int iAction, int iGiveH, int iRecvH, class CItem * pItem, BOOL bForceItemLog = FALSE) ;
	BOOL _bItemLog(int iAction, int iClientH, char * cName, class CItem * pItem);
	// v2.14 Log
	BOOL _bPKLog(int iAction, int iAttackerH, int iVictumH, char * cNPC) ;
	BOOL _bCheckGoodItem( class CItem * pItem ); 

	void _CheckStrategicPointOccupyStatus(char cMapIndex);
	void GetMapInitialPoint(int iMapIndex, short * pX, short * pY, char * pPlayerLocation = NULL);
	void AdminOrder_SummonDemon(int iClientH);
	int  iGetMaxHP(int iClientH);
	void _ClearQuestStatus(int iClientH);
	BOOL _bCheckItemReceiveCondition(int iClientH, class CItem * pItem);
	void SendItemNotifyMsg(int iClientH, WORD wMsgType, class CItem * pItem, int iV1);
	
	int _iTalkToNpcResult_WTower(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	int _iTalkToNpcResult_WHouse(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	int _iTalkToNpcResult_BSmith(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	int _iTalkToNpcResult_GShop(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	int _iTalkToNpcResult_GuildHall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	BOOL _bCheckIsQuestCompleted(int iClientH);
	void _CheckQuestEnvironment(int iClientH);
	void _SendQuestContents(int iClientH);
	void QuestAcceptedHandler(int iClientH);
	BOOL _bDecodeQuestConfigFileContents(char * pData, DWORD dwMsgSize);
	void CancelExchangeItem(int iClientH);
	BOOL bAddItem(int iClientH, class CItem * pItem, char cMode);
	void ConfirmExchangeItem(int iClientH);
	void SetExchangeItem(int iClientH, int iItemIndex, int iAmount);
	void ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName);
	void _BWM_Command_Shutup(char * pData);
	void _BWM_Init(int iClientH, char * pData);

	// v2.15 Manager program 2002-5-6
	void _Manager_Init(int iClientH, char * pData);
	void _Manager_Shutdown(int iClientH, char * pData);


	void CheckUniqueItemEquipment(int iClientH);
	void _SetItemPos(int iClientH, char * pData);
	void GetHeroMantleHandler(int iClientH);
	
	BOOL _bDecodeOccupyFlagSaveFileContents(char * pData, DWORD dwMsgSize);
	void GetOccupyFlagHandler(int iClientH);
	int  _iComposeFlagStatusContents(char * pData);
	void SetSummonMobAction(int iClientH, int iMode, DWORD dwMsgSize, char * pData = NULL);
	BOOL __bSetOccupyFlag(char cMapIndex, int dX, int dY, int iSide, int iEKNum, int iClientH, BOOL bAdminFlag);
	BOOL _bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID);
	void SetDownSkillIndexHandler(int iClientH, int iSkillIndex);
	int iGetComboAttackBonus(int iSkill, int iComboCount);
	int  _iGetWeaponSkillType(int iClientH);
	void AdminOrder_GetNpcStatus(int iClientH, char * pData, DWORD dwMsgSize);
	void CheckFireBluring(char cMapIndex, int sX, int sY);
	void NpcTalkHandler(int iClientH, int iWho);
	BOOL bDeleteMineral(int iIndex);
	void _CheckMiningAction(int iClientH, int dX, int dY);
	int iCreateMineral(char cMapIndex, int tX, int tY, char cLevel);
	void MineralGenerator();
	void LocalSavePlayerData(int iClientH);
	BOOL _bDecodePortionConfigFileContents(char * pData, DWORD dwMsgSize);
	void ReqCreatePortionHandler(int iClientH, char * pData);
	void _CheckAttackType(int iClientH, short * spType);
	BOOL bOnClose();
	void AdminOrder_SetInvi(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_Polymorph(int iClientH, char * pData, DWORD dwMsgSize);
	void ForceDisconnectAccount(char * pAccountName, WORD wCount);
	void NpcRequestAssistance(int iNpcH);
	void ToggleSafeAttackModeHandler(int iClientH);
	void AdminOrder_CheckIP(int iClientH, char * pData, DWORD dwMsgSize);
	void SetBerserkFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void SpecialEventHandler();
	int iGetPlayerRelationship_SendEvent(int iClientH, int iOpponentH);
	int iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH);
	int _iForcePlayerDisconect(int iNum);
	void AdminOrder_Teleport(int iClientH, char * pData, DWORD dwMsgSize);
	int iGetMapIndex(char * pMapName);
	//int iGetNpcRelationship(int iClientH, int iOpponentH);
	int iGetNpcRelationship(int iWhatH, int iRecvH);
	int iGetPlayerRelationship(int iClientH, int iOpponentH);
	int iGetWhetherMagicBonusEffect(short sType, char cWheatherStatus);
	void WhetherProcessor();
	int _iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius);
	void FishGenerator();
	void ReqGetFishThisTimeHandler(int iClientH);
	void AdminOrder_CreateFish(int iClientH, char * pData, DWORD dwMsgSize);
	void FishProcessor();
	int iCheckFish(int iClientH, char cMapIndex, short dX, short dY);
	BOOL bDeleteFish(int iHandle, int iDelMode);
	int  iCreateFish(char cMapIndex, short sX, short sY, short sDifficulty, class CItem * pItem, int iDifficulty, DWORD dwLastTime);
	void UserCommand_DissmissGuild(int iClientH, char * pData, DWORD dwMsgSize);
	// v1.4311-3 Changed function declaration Expire -> Ban
	void UserCommand_BanGuildsman(int iClientH, char * pData, DWORD dwMsgSize);
	// v1.4311-3 Added declaration: a GM reserves a fight zone for events.
	void AdminOrder_ReserveFightzone(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_CloseConn(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_CallGuard(int iClientH, char * pData, DWORD dwMsgSize);
	int iGetExpLevel(int iExp);
	void ___RestorePlayerRating(int iClientH);
	void CalcExpStock(int iClientH);
	void ResponseSavePlayerDataReplyHandler(char * pData, DWORD dwMsgSize);
	void NoticeHandler();
	BOOL bReadNotifyMsgListFile(char * cFn);
	void SetPlayerReputation(int iClientH, char * pMsg, char cValue, DWORD dwMsgSize);
	void ShutUpPlayer(int iClientH, char * pMsg, DWORD dwMsgSize);
	void CheckDayOrNightMode();
	BOOL bCheckBadWord(char * pString);
	BOOL bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType);
	void PoisonEffect(int iClientH, int iV1);
	void bSetNpcAttackMode(char * cName, int iTargetH, char cTargetType, BOOL bIsPermAttack);
	BOOL _bGetIsPlayerHostile(int iClientH, int sOwnerH);
	BOOL bAnalyzeCriminalAction(int iClientH, short dX, short dY, BOOL bIsCheck = FALSE);
	void RequestAdminUserMode(int iClientH, char * pData);
	int _iGetPlayerNumberOnSpot(short dX, short dY, char cMapIndex, char cRange);
	void CalcTotalItemEffect(int iClientH, int iEquipItemID, BOOL bNotify = TRUE);
	void ___RestorePlayerCharacteristics(int iClientH);
	void GetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize);
	void SetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize);
	void ToggleWhisperPlayer(int iClientH, char * pMsg, DWORD dwMsgSize);
	void CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, DWORD dwSize);
	int iCalcTotalWeight(int iClientH);
	void ReqRepairItemCofirmHandler(int iClientH, char cItemID, char * pString);
	void ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char * pString);
	void ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char * pString);
	void ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char * pItemName);
	void UseSkillHandler(int iClientH, int iV1, int iV2, int iV3);
	int  iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY);
	void ClearSkillUsingStatus(int iClientH);
	void DynamicObjectEffectProcessor();
	int _iGetTotalClients();
	void SendObjectMotionRejectMsg(int iClientH);
	void SetInvisibilityFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	BOOL bRemoveFromDelayEventList(int iH, char cType, int iEffectType);
	void DelayEventProcessor();
	BOOL bRegisterDelayEvent(int iDelayType, int iEffectType, DWORD dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3);
	int iGetFollowerNumber(short sOwnerH, char cOwnerType);
	int  _iCalcSkillSSNpoint(int iLevel);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	BOOL bCheckTotalSkillMasteryPoints(int iClientH, int iSkill);
	BOOL bSetItemToBankItem(int iClientH, class CItem * pItem);
	void NpcMagicHandler(int iNpcH, short dX, short dY, short sType);
	BOOL bCheckResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio);
	BOOL bCheckResistingMagicSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio);
	void Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
	void Effect_SpDown_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
	void Effect_HpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
	void Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, BOOL bExp, int iAttr = NULL);
	void UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID);
	void NpcBehavior_Stop(int iNpcH);
	// v2.15 
	void ItemDepleteHandler(int iClientH, short sItemIndex, BOOL bIsUseItemResult, BOOL bIsLog = TRUE);
	int _iGetArrowItemIndex(int iClientH);
	void RequestFullObjectData(int iClientH, char * pData);
	void DeleteNpc(int iNpcH);
	void CalcNextWayPointDestination(int iNpcH);
	void MobGenerator();
	void CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue);
	void CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue);
	void CheckDynamicObjectList();
	int  iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, DWORD dwLastTime, int iV1 = NULL);
	int _iCalcMaxLoad(int iClientH);
	void GetRewardMoneyHandler(int iClientH);
	void _PenaltyItemDrop(int iClientH, int iTotal, BOOL bIsSAattacked = FALSE,BOOL bItemDrop = FALSE);
	void ApplyCombatKilledPenalty(int iClientH, char cPenaltyLevel, BOOL bIsSAattacked = FALSE, BOOL bItemDrop = FALSE);
	void EnemyKillRewardHandler(int iAttackerH, int iClientH);
	void PK_KillRewardHandler(short sAttackerH, short sVictumH);
	void ApplyPKpenalty(short sAttackerH, short sVictumLevel);
	BOOL bSetItemToBankItem(int iClientH, short sItemIndex);
	void RequestRetrieveItemHandler(int iClientH, char * pData);
	void RequestCivilRightHandler(int iClientH, char * pData);
	BOOL bCheckLimitedUser(int iClientH);
	void LevelUpSettingsHandler(int iClientH, char * pData, DWORD dwMsgSize);
	// v1.4311-3 Declared the fight-zone reservation function FightzoneReserveHandler
	void FightzoneReserveHandler(int iClientH, char * pData, DWORD dwMsgSize);
	BOOL bCheckLevelUp(int iClientH);
	int iGetLevelExp(int iLevel);
	void TimeManaPointsUp(int iClientH);
	void TimeStaminarPointsUp(int iClientH);
	void Quit();
	BOOL __bReadMapInfo(int iMapIndex);

	int  _iGetSkillNumber(char * pSkillName);
	void TrainSkillResponse(BOOL bSuccess, int iClientH, int iSkillNum, int iSkillLevel);
	int _iGetMagicNumber(char * pMagicName, int * pReqInt, int * pCost);
	void RequestStudyMagicHandler(int iClientH, char * pName, BOOL bIsPurchase = TRUE);
	BOOL _bDecodeSkillConfigFileContents(char * pData, DWORD dwMsgSize);
	BOOL _bDecodeMagicConfigFileContents(char * pData, DWORD dwMsgSize);
	void ReleaseFollowMode(short sOwnerH, char cOwnerType);
	BOOL bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType);
	void RequestTeleportHandler(int iClientH, char * pData, char * cMapName = NULL, int dX = -1, int dY = -1);
	void PlayerMagicHandler(int iClientH, int dX, int dY, short sType, BOOL bItemEffect = FALSE, int iV1 = NULL);
	int  iClientMotion_Magic_Handler(int iClientH, short sX, short sY, char cDir);
	void SendMsgToGateServer(DWORD dwMsg, int iClientH, char * pData = NULL);
	void ToggleCombatModeHandler(int iClientH); 
	void GuildNotifyHandler(char * pData, DWORD dwMsgSize);
	void SendGuildMsg(int iClientH, WORD wNotifyMsgType, short sV1, short sV2, char * pString);
	void DelayEventProcess();
	void TimeHitPointsUp(int iClientH);
	void CalculateGuildEffect(int iVictimH, char cVictimType, short sAttackerH);
	void OnStartGameSignal();
	int iDice(int iThrow, int iRange);
	BOOL _bInitNpcAttr(class CNpc * pNpc, char * pNpcName, short sClass, char cSA);
	BOOL _bDecodeNpcConfigFileContents(char * pData, DWORD dwMsgSize);
	void ReleaseItemHandler(int iClientH, short sItemIndex, BOOL bNotice);
	void ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage);
	int  SetItemCount(int iClientH, char * pItemName, DWORD dwCount);
	int  SetItemCount(int iClientH, int iItemIndex, DWORD dwCount);
	DWORD dwGetItemCount(int iClientH, char * pName);
	void DismissGuildRejectHandler(int iClientH, char * pName);
	void DismissGuildApproveHandler(int iClientH, char * pName);
	void JoinGuildRejectHandler(int iClientH, char * pName);			    
	void JoinGuildApproveHandler(int iClientH, char * pName);
	void SendNotifyMsg(int iFromH, int iToH, WORD wMsgType, DWORD sV1, DWORD sV2, DWORD sV3, char * pString, DWORD sV4 = NULL, DWORD sV5 = NULL, DWORD sV6 = NULL, DWORD sV7 = NULL, DWORD sV8 = NULL, DWORD sV9 = NULL, char * pString2 = NULL);
	void GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName);
	void RequestPurchaseItemHandler(int iClientH, char * pItemName, int iNum);
	void ResponseDisbandGuildHandler(char * pData, DWORD dwMsgSize);
	void RequestDisbandGuildHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestCreateNewGuildHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void ResponseCreateNewGuildHandler(char * pData, DWORD dwMsgSize);
	int  iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir);
	
	BOOL bEquipItemHandler(int iClientH, short sItemIndex, BOOL bNotify = TRUE);
	BOOL _bAddClientItemList(int iClientH, class CItem * pItem, int * pDelReq);
	int  iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir);
	void DropItemHandler(int iClientH, short sItemIndex, int iAmount, char * pItemName, BOOL bByPlayer = FALSE);
	void ClientCommonHandler(int iClientH, char * pData);
	BOOL __fastcall bGetMsgQuene(char * pFrom, char * pData, DWORD * pMsgSize, int * pIndex, char * pKey);
	void MsgProcess();
	BOOL __fastcall bPutMsgQuene(char cFrom, char * pData, DWORD dwMsgSize, int iIndex, char cKey);
	void NpcBehavior_Flee(int iNpcH);
	int iGetDangerValue(int iNpcH, short dX, short dY);
	void NpcBehavior_Dead(int iNpcH);
	void NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage);
	int  iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, BOOL bNearAttack = FALSE, BOOL bIsDash = FALSE);
	void RemoveFromTarget(short sTargetH, char cTargetType, int iCode = NULL);
	void NpcBehavior_Attack(int iNpcH);
	void TargetSearch(int iNpcH, short * pTarget, char * pTargetType);
	void NpcBehavior_Move(int iNpcH);
	BOOL bGetEmptyPosition(short * pX, short * pY, char cMapIndex);
	char cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int * pError);
	int  iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, WORD wTargetObjectID, BOOL bRespose = TRUE, BOOL bIsDash = FALSE);
	void ChatMsgHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void NpcProcess();
	BOOL bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType, int * poX, int * poY, char * pWaypointList, RECT * pArea, int iSpotMobIndex, char cChangeSide, BOOL bHideGenMode, BOOL bIsSummoned = FALSE, BOOL bFirmBerserk = FALSE, BOOL bIsMaster = FALSE, int iGuildGUID = NULL);
	//BOOL bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sX, short sY);
	BOOL _bReadMapInfoFiles(int iMapIndex);
	
	BOOL _bGetIsStringIsNumber(char * pStr);
	BOOL _bInitItemAttr(class CItem * pItem, char * pItemName);
	// v2.17 2002-7-31 Allow creating items by their unique number.
	BOOL _bInitItemAttr(class CItem * pItem, int iItemID);
	BOOL bReadProgramConfigFile(char * cFn);
	BOOL bLoadGameDataFiles();	// WORLD-003: game_data_files.cpp
	void GameProcess();
	void InitPlayerData(int iClientH, char * pData, DWORD dwSize);
	void ResponsePlayerDataHandler(char * pData, DWORD dwSize);
	// v2.14 Changed to send the GM log to the world server.
	BOOL bSendMsgToLS(DWORD dwMsg, int iClientH, BOOL bFlag = TRUE,char * pData = NULL );
	void CheckClientResponseTime();
	void OnTimer(char cType);
	int iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData);
	void SendEventToNearClient_TypeB(DWORD dwMsgID, WORD wMsgType, char cMapIndex, short sX, short sY, short sV1 = 0, short sV2 = 0, short sV3 = 0, short sV4 = 0);
	void SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, DWORD dwMsgID, WORD wMsgType, short sV1, short sV2, short sV3);
	void DeleteClient(int iClientH, BOOL bSave, BOOL bNotify, BOOL bCountLogout = TRUE, BOOL bForceCloseConn = FALSE);
	int  iComposeInitMapData(short sX, short sY, int iClientH, char * pData);
	void RequestInitDataHandler(int iClientH, char * pData, char cKey, BOOL bIsNoNameCheck = FALSE);
	void RequestInitPlayerHandler(int iClientH, char * pData, char cKey);
	int  iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, BOOL bIsRun);
	void ClientMotionHandler(int iClientH, char * pData);
	void DisplayInfo(HDC hdc);
	void OnClientRead(int iClientH);
	BOOL bInit();
	void OnClientSocketEvent(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL bAccept(class XSocket * pXSock);
	// v1.4311-3 Declared the ticket function .. void GetFightzoneTicketHandler(int iClientH);
	void GetFightzoneTicketHandler(int iClientH);
	// v1.4311-3 Reset fight-zone reservations every 2 hours.
	void FightzoneReserveProcessor() ;

	// 2002-10-23 Item Event
	BOOL NpcDeadItemGeneratorWithItemEvent(int iNpcH, short sAttackerH, char cAttackerType);
	// 2002-10-24 Items in the Item Event List with Type 0 are excluded from normal item generation.
	BOOL bCheckInItemEventList(int iItemID, int iNpcH);

	// 2002-12-6  Added the Teleport feature
	BOOL _bDecodeTeleportListConfigFileContents(char * pData, DWORD dwMsgSize);
	// 2002-12-6  Added the Teleport feature
	void RequestTeleportListHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestChargedTeleportHandler(int iClientH, char *pData, DWORD dwMsgSize);

	CGame(HWND hWnd);
	virtual ~CGame();

	char m_cServerName[12];
	char m_cGameServerAddr[16];
	char m_cLogServerAddr[16];
	char m_cGateServerAddr[16];
	int  m_iGameServerPort;
	int  m_iLogServerPort;
	int  m_iGateServerPort;
	int  m_iWorldLogServerPort;

	int  m_iLimitedUserExp, m_iLevelExp20;

//private:
	BOOL _bDecodeItemConfigFileContents(char * pData, DWORD dwMsgSize);
	int _iComposePlayerDataFileContents(int iClientH, char * pData);
	BOOL _bDecodePlayerDatafileContents(int iClientH, char * pData, DWORD dwSize);
	BOOL _bRegisterMap(char * pName);

	class CClient * m_pClientList[DEF_MAXCLIENTS];
	class CNpc    * m_pNpcList[DEF_MAXNPCS];
	class CMap    * m_pMapList[DEF_MAXMAPS];
	class CDynamicObject * m_pDynamicObjectList[DEF_MAXDYNAMICOBJECTS];
	class CDelayEvent    * m_pDelayEventList[DEF_MAXDELAYEVENTS];

	class CMsg    * m_pMsgQuene[DEF_MSGQUENESIZE];
	int             m_iQueneHead, m_iQueneTail;
	int             m_iTotalMaps;
	class XSocket * m_pMainLogSock, * m_pGateSock;
	int				m_iGateSockConnRetryTimes;
	class CMisc     m_Misc;
	BOOL			m_bIsGameStarted;
	BOOL            m_bIsLoginLinkAvailable;	// WORLD-003: the Login link is registered; replaces the log and gate socket flags
	BOOL			m_bIsItemAvailable, m_bIsBuildItemAvailable, m_bIsNpcAvailable, m_bIsMagicAvailable;
	BOOL			m_bIsSkillAvailable, m_bIsPortionAvailable, m_bIsQuestAvailable, m_bIsWLServerAvailable ;
	class CItem   * m_pItemConfigList[DEF_MAXITEMTYPES];
	class CNpc    * m_pNpcConfigList[DEF_MAXNPCTYPES];
	class CMagic  * m_pMagicConfigList[DEF_MAXMAGICTYPE];
	class CSkill  * m_pSkillConfigList[DEF_MAXSKILLTYPE];
	class CQuest  * m_pQuestConfigList[DEF_MAXQUESTTYPE];
	char            m_pMsgBuffer[DEF_MSGBUFFERSIZE+1];

	// 2002-12-6  Added the Teleport feature
	class CTeleport * m_pTeleportConfigList[DEF_MAXTELEPORTLIST];

	HWND  m_hWnd;
	int   m_iTotalClients, m_iMaxClients, m_iTotalGameServerClients, m_iTotalGameServerMaxClients;
	SYSTEMTIME m_MaxUserSysTime;

	BOOL  m_bF1pressed, m_bF4pressed, m_bF12pressed;
	BOOL  m_bOnExitProcess;
	DWORD m_dwExitProcessTime;

	DWORD m_dwWhetherTime, m_dwGameTime1, m_dwGameTime2, m_dwGameTime3, m_dwGameTime4, m_dwGameTime5, m_dwGameTime6, m_dwFishTime;

	BOOL  m_cDayOrNight;		// 1 is day, 2 is night
 	int   m_iSkillSSNpoint[102];

	class CMsg * m_pNoticeMsgList[DEF_MAXNOTIFYMSGS];
	int   m_iTotalNoticeMsg, m_iPrevSendNoticeMsg;
	DWORD m_dwNoticeTime, m_dwSpecialEventTime;
	BOOL  m_bIsSpecialEventTime;
	char  m_cSpecialEventType;

	int   m_iLevelExpTable[300];
 	class CFish * m_pFish[DEF_MAXFISHS];
	class CPortion * m_pPortionConfigList[DEF_MAXPORTIONTYPES];

	BOOL  m_bIsServerShutdowned;
	char  m_cShutDownCode;
	class CMineral * m_pMineral[DEF_MAXMINERALS];

	int   m_iMiddlelandMapIndex;	// Map index of middleland, the battlefield: -1 means there is no middleland map
	int   m_iAresdenMapIndex;		// Aresden map index
	int	  m_iElvineMapIndex;		// Elvine map index
	int   m_iAresdenOccupyTiles;
	int   m_iElvineOccupyTiles;
	int   m_iCurMsgs, m_iMaxMsgs;

	// v1.4311-3 Declared the variable holding the reserved time DWORD m_dwCanFightzoneReserveTime
	DWORD m_dwCanFightzoneReserveTime ;
	// v1.4311-3 Declared the per-fight-zone reservation state int  m_iFightZoneReserve[DEF_MAXFIGHTZONE];
	int  m_iFightZoneReserve[DEF_MAXFIGHTZONE] ;
	// v1.4311-3 Declared: no forced recall by GM command inside a fight zone.
	int  m_iFightzoneNoForceRecall  ;

	struct {
		__int64 iFunds;
		__int64 iCrimes;
		__int64 iWins;

	} m_stCityStatus[3];
	
	int	  m_iStrategicStatus;
	
	class XSocket * m_pSubLogSock[DEF_MAXSUBLOGSOCK];
	int   m_iSubLogSockInitIndex;
	BOOL  m_bIsSubLogSockAvailable[DEF_MAXSUBLOGSOCK];
	int	  m_iCurSubLogSockIndex;
	int   m_iSubLogSockFailCount;
	int   m_iSubLogSockActiveCount;	// v1.5 Number of sub-log-sockets currently connected
	int   m_iAutoRebootingCount;	// v1.5 Number of automatic reboots

	class CBuildItem * m_pBuildItemList[DEF_MAXBUILDITEMS];
	class CItem * m_pDupItemIDList[DEF_MAXDUPITEMID];

	char * m_pNoticementData;
	DWORD  m_dwNoticementDataSize;

	DWORD  m_dwMapSectorInfoTime;
	int    m_iMapSectorInfoUpdateCount;

	// For Crusade handling
	int	   m_iCrusadeCount;			// 0 is all-out war standby. 1 starts the countdown.
	BOOL   m_bIsCrusadeMode;		// Crusade mode

	struct {
		char cMapName[11];			// Map where it is placed
		char cType;					// Building type. Actually the NPC type.
		int  dX, dY;				// Position on the map where it is placed

	} m_stCrusadeStructures[DEF_MAXCRUSADESTRUCTURES];

	
	int m_iCollectedMana[3];
	int m_iAresdenMana, m_iElvineMana;

	class CTeleportLoc m_pGuildTeleportLoc[DEF_MAXGUILDS];
	//

	WORD  m_wServerID_GSS;
	char  m_cGateServerStockMsg[DEF_MAXGATESERVERSTOCKMSGSIZE];
	int   m_iIndexGSS;

	struct {
		int iCrashedStructureNum;
		int iStructureDamageAmount;
		int iCasualties;
	} m_stMeteorStrikeResult;

	// Structure holding middleland info. Servers that do not run middleland receive and update it.
	struct {
		char cType;			// NULL means not defined.
		char cSide;			// Side
		short sX, sY;		// Placed position
	} m_stMiddleCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
	int m_iTotalMiddleCrusadeStructures;
 
	// Holds the indexes of connected clients without gaps. A 0 marks the end of the list
	int m_iClientShortCut[DEF_MAXCLIENTS+1];

	int m_iNpcConstructionPoint[DEF_MAXNPCTYPES];
	DWORD m_dwCrusadeGUID;
	int   m_iCrusadeWinnerSide;   
	int	  m_iWinnerSide;		//v2.19 2002-11-15 
	int	  m_iNonAttackArea;		//v2.19 2002-11-19 Added so the beginner middleland can never be attacked.

	int   m_iPlayerMaxLevel;
	// v2.15 2002-5-21
	int   m_iWorldMaxUser;

	// v2.17 2002-7-15
	short m_sForceRecallTime;

	BOOL  m_bIsCrusadeWarStarter;
	int   m_iLatestCrusadeDayOfWeek;
	int   m_iFinalShutdownCount;

	struct {
		int iDay;
		int iHour;
		int iMinute;
	} m_stCrusadeWarSchedule[DEF_MAXSCHEDULE];

	struct {
		int iTotalMembers;
		int iIndex[DEF_MAXPARTYMEMBERS];
	} m_stPartyInfo[DEF_MAXPARTYNUM];

	class CItem * m_pGold;

	// 2002-09-09 #1
	// Variable that checks the ITEM List arrived before the NPCITEM List
	bool	m_bReceivedItemList;

private:
	int __iSearchForQuest(int iClientH, int iWho, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	int _iTalkToNpcResult_Cityhall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	void _ClearExchangeStatus(int iClientH);
	int _iGetItemSpaceLeft(int iClientH);

};

#endif // !defined(AFX_GAME_H__C3D29FC5_755B_11D2_A8E6_00001C7030A6__INCLUDED_)
