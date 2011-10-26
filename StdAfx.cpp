// stdafx.cpp : source file that includes just the standard includes
//	rumfc.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// This to cover the Endless-Globals-in-the-ClassView bug in VC6
// that occurs when there are no global variables.
// Can be removed if real global variables are added.
char g_ucDummy;
