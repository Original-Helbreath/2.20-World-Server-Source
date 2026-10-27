// TeleportLoc.h: interface for the CTeleportLoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELEPORTLOC_H__D4288400_F8C0_11D2_9B6C_00001C7030A6__INCLUDED_)
#define AFX_TELEPORTLOC_H__D4288400_F8C0_11D2_9B6C_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>

class CTeleportLoc  
{
public:
	CTeleportLoc(); 
	virtual ~CTeleportLoc();
												  
	short m_sSrcX, m_sSrcY;

	char  m_cDestMapName[11],  m_cDir;
	char  m_cDestMapName2[11];
	short m_sDestX,  m_sDestY;
	short m_sDestX2, m_sDestY2;

	int   m_iV1;			// Guild ID
	int	  m_iV2;			// Buildings summoned in all-out war

	// 2002-12-24 Limit the NPCs (excluding buildings) that can be summoned in all-out war. (Taiwan request)
	int   m_iNumSummonNpc;	// NPCs summoned in all-out war (buildings excluded.)

	DWORD m_dwTime, m_dwTime2;

};

#endif // !defined(AFX_TELEPORTLOC_H__D4288400_F8C0_11D2_9B6C_00001C7030A6__INCLUDED_)
