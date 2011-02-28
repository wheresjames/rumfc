/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl

#include "stdafx.h"

CSortListCtrl::CSortListCtrl(CListCtrl * _pWnd, const int _iCol, const bool _bIsNumeric)
{
	iCol = _iCol;
	pWnd = _pWnd;
	bIsNumeric = _bIsNumeric;

	ASSERT(pWnd);
	int max = pWnd->GetItemCount();
	DWORD dw;
	CString txt;
	if (bIsNumeric)
	{
		for (int t = 0; t < max; t++)
		{
			dw = pWnd->GetItemData(t);
			txt = pWnd->GetItemText(t, iCol);
			pWnd->SetItemData(t, (DWORD) new CSortItemInt(dw, txt));
		}
	}
	else
	{
		for (int t = 0; t < max; t++)
		{
			dw = pWnd->GetItemData(t);
			txt = pWnd->GetItemText(t, iCol);
			pWnd->SetItemData(t, (DWORD) new CSortItem(dw, txt));
		}
	}
}

CSortListCtrl::~CSortListCtrl()
{
	ASSERT(pWnd);
	int max = pWnd->GetItemCount();
	if (bIsNumeric)
	{
		CSortItemInt * pItem;
		for (int t = 0; t < max; t++)
		{
			pItem = (CSortItemInt *) pWnd->GetItemData(t);
			ASSERT(pItem);
			pWnd->SetItemData(t, pItem->dw);
			delete pItem;
		}
	}
	else
	{
		CSortItem * pItem;
		for (int t = 0; t < max; t++)
		{
			pItem = (CSortItem *) pWnd->GetItemData(t);
			ASSERT(pItem);
			pWnd->SetItemData(t, pItem->dw);
			delete pItem;
		}
	}
}

void
CSortListCtrl::Sort(const bool bAsc)
{
	if (bIsNumeric)
	{
		if (bAsc)
			pWnd->SortItems(CompareAscI, 0L);
		else
			pWnd->SortItems(CompareDesI, 0L);
	}
	else
	{
		if (bAsc)
			pWnd->SortItems(CompareAsc, 0L);
		else
			pWnd->SortItems(CompareDes, 0L);
	}
}

int CALLBACK CSortListCtrl::CompareAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortItem * i1 = (CSortItem *) lParam1;
	CSortItem * i2 = (CSortItem *) lParam2;
	ASSERT(i1 && i2);
	return i1->txt.CompareNoCase(i2->txt);
}

int CALLBACK CSortListCtrl::CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortItem * i1 = (CSortItem *) lParam1;
	CSortItem * i2 = (CSortItem *) lParam2;
	ASSERT(i1 && i2);
	return i2->txt.CompareNoCase(i1->txt);
}

int CALLBACK CSortListCtrl::CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortItemInt * i1 = (CSortItemInt *) lParam1;
	CSortItemInt * i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);
	if (i1->dInt == i2->dInt) return 0;
	return i1->dInt > i2->dInt ? 1 : -1;
}

int CALLBACK CSortListCtrl::CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortItemInt * i1 = (CSortItemInt *) lParam1;
	CSortItemInt * i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);
	if (i1->dInt == i2->dInt) return 0;
	return i1->dInt < i2->dInt ? 1 : -1;
}

CSortListCtrl::CSortItem::CSortItem(const DWORD _dw, const CString & _txt)
{
	dw = _dw;
	txt = _txt;
}

CSortListCtrl::CSortItem::~CSortItem()
{
}

CSortListCtrl::CSortItemInt::CSortItemInt(const DWORD _dw, const CString & _txt)
{
//	iInt = atoi(_txt);
	dInt = strtod( _txt, NULL );
	dw = _dw;
}

