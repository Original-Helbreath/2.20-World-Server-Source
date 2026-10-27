// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
#define AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_

#include <windows.h>
#include <winbase.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
										   
#define DEF_MAXITEMEQUIPPOS		15
#define DEF_EQUIPPOS_NONE		0		// Cannot be equipped.
#define DEF_EQUIPPOS_HEAD		1
#define DEF_EQUIPPOS_BODY		2
#define DEF_EQUIPPOS_ARMS		3
#define DEF_EQUIPPOS_PANTS		4
#define DEF_EQUIPPOS_LEGGINGS	5
#define DEF_EQUIPPOS_NECK		6
#define DEF_EQUIPPOS_LHAND		7
#define DEF_EQUIPPOS_RHAND		8
#define DEF_EQUIPPOS_TWOHAND	9
#define DEF_EQUIPPOS_RFINGER	10
#define DEF_EQUIPPOS_LFINGER	11
#define DEF_EQUIPPOS_BACK		12
#define DEF_EQUIPPOS_FULLBODY	13     //Head body arm boots pants mantle
 
#define DEF_ITEMTYPE_NOTUSED	-1	// v1.4 An item used before but not now: replaced by the reward money.
#define DEF_ITEMTYPE_NONE		 0
#define DEF_ITEMTYPE_EQUIP		 1
#define DEF_ITEMTYPE_APPLY		 2
#define DEF_ITEMTYPE_USE_DEPLETE 3       
#define DEF_ITEMTYPE_INSTALL	 4
#define DEF_ITEMTYPE_CONSUME	 5
#define DEF_ITEMTYPE_ARROW		 6
#define DEF_ITEMTYPE_EAT		 7
#define DEF_ITEMTYPE_USE_SKILL   8
#define DEF_ITEMTYPE_USE_PERM    9
#define DEF_ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX	10
#define DEF_ITEMTYPE_USE_DEPLETE_DEST			11
#define DEF_ITEMTYPE_MATERIAL					12


#define DEF_ITEMEFFECTTYPE_NONE				0
#define DEF_ITEMEFFECTTYPE_ATTACK			1		// Attack value: value1 D value2 + value3
#define DEF_ITEMEFFECTTYPE_DEFENSE			2		// Defence
#define DEF_ITEMEFFECTTYPE_ATTACK_ARROW		3		// An attack weapon that uses arrows. The weapon itself has no attack power.
#define DEF_ITEMEFFECTTYPE_HP   		    4		// HP effect
#define DEF_ITEMEFFECTTYPE_MP   		    5		// MP effect
#define DEF_ITEMEFFECTTYPE_SP   		    6		// SP effect
#define DEF_ITEMEFFECTTYPE_HPSTOCK 		    7		// HP effect. Does not take effect immediately.
#define DEF_ITEMEFFECTTYPE_GET			    8		// Obtained. Fish, minerals, etc.
#define DEF_ITEMEFFECTTYPE_STUDYSKILL		9		// An item that teaches a skill.
#define DEF_ITEMEFFECTTYPE_SHOWLOCATION		10		// An item that shows the position.
#define DEF_ITEMEFFECTTYPE_MAGIC			11		// An item that gives a magic effect when used
#define DEF_ITEMEFFECTTYPE_CHANGEATTR		12		// Changes the player's attributes: sex, hair, skin colour, hair style
#define DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE	13		// Attack effect plus mana saving. (for magic)
#define DEF_ITEMEFFECTTYPE_ADDEFFECT	    14		// Extra effect
#define DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE	15		// Reduces damage from magic attacks. Rings and necklaces
#define DEF_ITEMEFFECTTYPE_OCCUPYFLAG		16		// Occupation flag
#define DEF_ITEMEFFECTTYPE_DYE				17		// Dye.
#define DEF_ITEMEFFECTTYPE_STUDYMAGIC		18		// An item that teaches a spell.
#define DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN	19		// An item with an attack effect that lowers max HP and HP recovery
#define DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE	20		// An item with an attack effect plus physical defence like armour
#define DEF_ITEMEFFECTTYPE_MATERIAL_ATTR	21		// Weapon crafting material attribute
#define DEF_ITEMEFFECTTYPE_FIRMSTAMINAR		22		// Stamina-fixing item
#define DEF_ITEMEFFECTTYPE_LOTTERY			23		// Lottery item

#define DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY		24	// Attack item special effect
#define DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY	25	// Armour item special effect
#define DEF_ITEMEFFECTTYPE_ALTERITEMDROP		26	// Drops instead of another item.

#define DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT		27	// Construction kit
#define DEF_ITEMEFFECTTYPE_WARM					28	// Thaws a frozen body. v2.172 2002-7-5
#define DEF_ITEMEFFECTTYPE_DEFENSE_ANTIMINE		29	// Defence v2.172 2002-7-5
#define DEF_ITEMEFFECTTYPE_ITEMFARM				30  //v2.19 2002-12-16 Farming skill

#define DEF_ITET_UNIQUE_OWNER				1		// ItemTouchEffect: an item with an owner
#define DEF_ITET_ID							2		// Just the item ID
#define DEF_ITET_DATE						3		// An item with a set usable date


class CItem  
{
public:

	CItem();
	virtual ~CItem();

	char  m_cName[21];
	
	short m_sIDnum;					// Item unique number
	char  m_cItemType;
	char  m_cEquipPos;
	short m_sItemEffectType;
	short m_sItemEffectValue1, m_sItemEffectValue2, m_sItemEffectValue3;
	short m_sItemEffectValue4, m_sItemEffectValue5, m_sItemEffectValue6;
	WORD  m_wMaxLifeSpan;
	short m_sSpecialEffect;
	
	//short m_sSM_HitRatio, m_sL_HitRatio;
	//v1.432 Hit rate modifier not used. A special ability value goes here instead.
	short m_sSpecialEffectValue1, m_sSpecialEffectValue2;

	short m_sSprite;
	short m_sSpriteFrame;

	char  m_cApprValue;
	char  m_cSpeed;

	DWORD m_wPrice;
	WORD  m_wWeight;
	short m_sLevelLimit;
	char  m_cGenderLimit;

	short m_sRelatedSkill;
	
	char  m_cCategory;
	BOOL  m_bIsForSale;
	// 

	DWORD m_dwCount;
	short m_sTouchEffectType;
	short m_sTouchEffectValue1, m_sTouchEffectValue2, m_sTouchEffectValue3;
	char  m_cItemColor; // Changed to the item colour in v1.4.
	short m_sItemSpecEffectValue1, m_sItemSpecEffectValue2, m_sItemSpecEffectValue3;
	// v2.16 2002-5-23 Fixed
	int  m_wCurLifeSpan;
	DWORD m_dwAttribute;				// aaaa bbbb cccc dddd eeee ffff xxxx xxx1 
										// 1: Custom-Made Item flag 
										// a: extra damage
										// b: RESERVED
										// c: special item property Flag
										// d: special item property level
										// e: special item extra property Flag
										// f: special item extra property level
										 

};

/*
Content needed in the item definition file - for the game server

char cItemType
 - Item type. Whether it is equipped or consumed

char cEquipPos
 - If equipped, its slot

short sItemEffectType     
  - Item effect type.

short sItemEffectValue1, 2, 3 ... 
 - Item effect values 1, 2, 3...

WORD wMaxLifeSpan
 - Item max lifespan

short sMaxFixCount
 - Max repair count

short sSprite
short sSpriteFrame
 - Item sprite number

WORD  wPrice 
 - Item price
WORD  wWeight
 - Item weight
short sLevelLimit
 - Item level limit
char  cGenderLimit
 - Item sex limit

short m_SM_HitRatio, m_sL_HitRatio
 - Per-target hit modifier (for weapons)

short sRelatedSkill
 - Linked Skill

Info to store in the player data file - for the log server

DWORD dwCount
 - Item count. (meaningful only for arrows, potions, Gold and such of one kind)

WORD wCurLifeSpan
 - Current remaining lifespan

short sCurFixCount
 - Repairs done so far

short sTouchEffectType
 - Effect type that fires when the item is first touched

short sTouchEffectValue1, 2, 3...
 - Touch effect value

short sItemSpecEffectType
 - Item special effect type (ex: an enchanted weapon may affect attack or defence)

short sItemSpecEffectValue1, 2, 3...
 - Item special effect values 1, 2, 3...


For clothing items (Equip Position Body, Leggings or Arms)
sItemEffectValue4: the stat limit type of the clothing.

  10: Str
  11: Dex
  12: Vit
  13: Int
  14: Mag
  15: Chr

sItemEffectValue5: the stat limit level of the clothing



*/

#endif // !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
