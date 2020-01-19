#include "../TRT_CablingSvc.h"
#include "../TRT_FillCablingData_SR1_ECC.h"
#include "../TRT_FillCablingData_SR1.h"
#include "../TRT_FillCablingData_TB04.h"
#include "../TRT_FillCablingData_DC1.h"
#include "../TRT_FillCablingData_DC2.h"
#include "../TRT_FillCablingData_DC3.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY( TRT_CablingSvc )

DECLARE_TOOL_FACTORY( TRT_FillCablingData_DC1 )
DECLARE_TOOL_FACTORY( TRT_FillCablingData_DC2 )
DECLARE_TOOL_FACTORY( TRT_FillCablingData_DC3 )
DECLARE_TOOL_FACTORY( TRT_FillCablingData_SR1 )
DECLARE_TOOL_FACTORY( TRT_FillCablingData_TB04 )
DECLARE_TOOL_FACTORY( TRT_FillCablingData_SR1_ECC )



DECLARE_FACTORY_ENTRIES( TRT_Cabling ) 
{
  DECLARE_SERVICE( TRT_CablingSvc )
  DECLARE_TOOL( TRT_FillCablingData_DC1 )
  DECLARE_TOOL( TRT_FillCablingData_DC2 )
  DECLARE_TOOL( TRT_FillCablingData_DC3 )
  DECLARE_TOOL( TRT_FillCablingData_SR1 )
  DECLARE_TOOL( TRT_FillCablingData_TB04 )
  DECLARE_TOOL( TRT_FillCablingData_SR1_ECC )
}
