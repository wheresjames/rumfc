/*******************************************************************
*
* CSortListCtrl ( originally CSortClass )
*
* Author: Iuri Apollonio
*
* http://www.codeguru.com/Cpp/controls/listview/sorting/article.php/c1067/
*
	USE:
	
	void CMyListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

		// bAscending will be use to order from lower to higher or higher to lower
		bool bByData = true;
		bool bAscending = true;

		CSortClass csc(this, pNMListView->iSubItem, bIsNumeric );
		csc.Sort( bAscending );

		*pResult = 0;
	}
*
*
*******************************************************************/

#pragma once

class CSortListCtrl
{
public:
	CSortListCtrl(CListCtrl * _pWnd, const int _iCol, const bool _bIsNumeric);
	virtual ~CSortListCtrl();

	int iCol;
	CListCtrl * pWnd;
	bool bIsNumeric;
	void Sort(const bool bAsc);

	static int CALLBACK CompareAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	static int CALLBACK CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

public:
	class CSortItem
	{
	public:
		virtual  ~CSortItem();
		CSortItem(const DWORD _dw, const CString &_txt);
		CString txt;
		DWORD dw;
	};
	class CSortItemInt
	{
	public:
		CSortItemInt(const DWORD _dw, const CString &_txt);
		//int iInt ;
		double dInt;
		DWORD dw;
	};
};