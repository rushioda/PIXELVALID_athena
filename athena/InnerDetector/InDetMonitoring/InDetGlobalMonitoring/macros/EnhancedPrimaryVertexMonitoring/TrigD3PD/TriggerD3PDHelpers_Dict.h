/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
* TriggerD3PDHelpers_Dict.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error TriggerD3PDHelpers_Dict.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#define G__PRIVATE_GVALUE
#include "G__ci.h"
extern "C" {
extern void G__cpp_setup_tagtableTriggerD3PDHelpers_Dict();
extern void G__cpp_setup_inheritanceTriggerD3PDHelpers_Dict();
extern void G__cpp_setup_typetableTriggerD3PDHelpers_Dict();
extern void G__cpp_setup_memvarTriggerD3PDHelpers_Dict();
extern void G__cpp_setup_globalTriggerD3PDHelpers_Dict();
extern void G__cpp_setup_memfuncTriggerD3PDHelpers_Dict();
extern void G__cpp_setup_funcTriggerD3PDHelpers_Dict();
extern void G__set_cpp_environmentTriggerD3PDHelpers_Dict();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "ChainGroup.h"
#include "ChainGroupHandling.h"
#include "Conditions.h"
#include "ConfigAccess.h"
#include "DataAccess.h"
#include "IConfigAccess.h"
#include "IDataAccess.h"
#include "TrigConfigSvcD3PD.h"
#include "TrigDecisionToolD3PD.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TClass;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TBuffer;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TMemberInspector;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TObject;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TNamed;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_vectorlEshortcOallocatorlEshortgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_vectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_string;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TString;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PD;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigDefs;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigDefscLcLDecisionTypes;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigConfigSvcD3PD;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrig;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLIDataAccess;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLChainGroupHandling;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLChainGroup;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_vectorlEstringcOallocatorlEstringgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_reverse_iteratorlEvectorlEstringcOallocatorlEstringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_vectorlEintcOallocatorlEintgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_reverse_iteratorlEvectorlEintcOallocatorlEintgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLIDataAccesscLcLL1ResultType;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLIDataAccesscLcLHLTResultType;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLIConfigAccess;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_lesslEstringgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_allocatorlEpairlEconstsPstringcOintgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOintcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOintgRsPgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_pairlEstringcOintgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOintcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOintgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOintcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOintgRsPgRsPgRcLcLreverse_iterator;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_allocatorlEpairlEconstsPstringcOfloatgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOfloatcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOfloatgRsPgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_pairlEstringcOfloatgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOfloatcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOfloatgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOfloatcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOfloatgRsPgRsPgRcLcLreverse_iterator;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_pairlEintcOintgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TTree;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_TBranch;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_pairlEintcOpairlEintcOintgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_allocatorlEpairlEconstsPstringcOstringgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOstringcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOstringgRsPgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_pairlEstringcOstringgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOstringcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOstringgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEstringcOstringcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOstringgRsPgRsPgRcLcLreverse_iterator;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_pairlEmaplEstringcOstringcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOstringgRsPgRsPgRcLcLiteratorcOboolgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_maplEpairlEintcOpairlEintcOintgRsPgRcOintcOlesslEpairlEintcOpairlEintcOintgRsPgRsPgRcOallocatorlEpairlEconstsPpairlEintcOpairlEintcOintgRsPgRcOintgRsPgRsPgR;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLConfigAccess;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigcLcLDataAccess;
extern G__linked_taginfo G__TriggerD3PDHelpers_DictLN_D3PDcLcLTrigDecisionToolD3PD;

/* STUB derived class for protected member access */
typedef map<string,int,less<string>,allocator<pair<const string,int> > > G__maplEstringcOintcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOintgRsPgRsPgR;
typedef map<string,float,less<string>,allocator<pair<const string,float> > > G__maplEstringcOfloatcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOfloatgRsPgRsPgR;
typedef map<string,string,less<string>,allocator<pair<const string,string> > > G__maplEstringcOstringcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOstringgRsPgRsPgR;