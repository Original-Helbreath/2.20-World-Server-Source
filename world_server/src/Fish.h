// Fish.h: interface for the CFish class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FISH_H__B43430CE_E9DE_45BF_9E17_EDF084B0BADD__INCLUDED_)
#define AFX_FISH_H__B43430CE_E9DE_45BF_9E17_EDF084B0BADD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <winbase.h>
#include "item.h"

class CFish  
{
public:

	CFish(char cMapIndex, short sX, short sY, short sType, class CItem * pItem, int iDifficulty);
	virtual ~CFish();

	char  m_cMapIndex;
	short m_sX, m_sY;			// Position on the map

	short m_sType;				// Fish type
	class CItem * m_pItem;		// Item given when caught (probably a fish)

	short m_sDynamicObjectHandle;

	short m_sEngagingCount;		// Number of players currently after this fish
	int   m_iDifficulty;		// Difficulty. Must be at least 1.
};

#endif // !defined(AFX_FISH_H__B43430CE_E9DE_45BF_9E17_EDF084B0BADD__INCLUDED_)
