// Quest.h: interface for the CQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEST_H__3E61C780_FF48_11D3_9DC2_00A0CC5B45EE__INCLUDED_)
#define AFX_QUEST_H__3E61C780_FF48_11D3_9DC2_00A0CC5B45EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>


#define DEF_QUESTTYPE_MONSTERHUNT				1		// Normal monster hunting
#define DEF_QUESTTYPE_MONSTERHUNT_TIMELIMIT		2		// Timed monster hunting
#define DEF_QUESTTYPE_ASSASSINATION 			3		// Assassination
#define DEF_QUESTTYPE_DELIVERY					4		// Delivery: deliver an item to a given place.
#define DEF_QUESTTYPE_ESCORT					5		// Protection: protect a given character.
#define DEF_QUESTTYPE_GUARD						6		// Defend a given area from enemy attack.
#define DEF_QUESTTYPE_GOPLACE					7		// Go to a given place. Enemy infiltration: quests that go to dungeons or other hard places
#define DEF_QUESTTYPE_BUILDSTRUCTURE			8		// Structure placement mission
#define DEF_QUESTTYPE_SUPPLYBUILDSTRUCTURE		9		// Structure placement supply mission
#define DEF_QUESTTYPE_STRATEGICSTRIKE			10		// Tactical bombing mission
#define DEF_QUESTTYPE_SENDTOBATTLE				11		// Mission to join the battle at once
#define DEF_QUESTTYPE_SETOCCUPYFLAG				12		// Mission to plant a territory flag

class CQuest  
{
public:
	
	char m_cSide;				// Which side is the mission for?
	
	int m_iType;				// Quest type
	int m_iTargetType;			// Quest target type. The value depends on each Type.
	int m_iMaxCount;			// Max completion count

	int m_iFrom;				// Type of NPC that gives the Quest
	
	int m_iMinLevel;			// Minimum level to receive the Quest.
	int m_iMaxLevel;			// Maximum level to receive the Quest

	int m_iRequiredSkillNum;	// Skill number needed for the condition
	int m_iRequiredSkillLevel;	// Skill level needed for the condition

	int m_iTimeLimit; 			// Time limit remaining
	int m_iAssignType;			// Quest assign type. -1 anytime. 1 only during a Crusade all-out war.

								// Reward type and quantity. One of the 3 is chosen at random. Index 0 is unused.
	int m_iRewardType[4]; 
	int m_iRewardAmount[4];

	int m_iContribution;		// Contribution
	int m_iContributionLimit;	// Max contribution limit

	int m_iResponseMode;		// Response mode: 0(ok) 1(Accept/Decline) 2(Next)

	char m_cTargetName[21];		// Name the quest targets. A map or character name
	int  m_sX, m_sY, m_iRange;	// Map location the quest targets

	int  m_iQuestID;			// Quest ID. In case the quest content changes.

	int  m_iReqContribution;	// Minimum contribution to take the quest.


	CQuest();
	virtual ~CQuest();

};

#endif // !defined(AFX_QUEST_H__3E61C780_FF48_11D3_9DC2_00A0CC5B45EE__INCLUDED_)
