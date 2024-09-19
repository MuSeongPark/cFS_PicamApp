

#ifndef _picam_app_h_
#define _picam_app_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "picam_app_perfids.h"
#include "picam_app_msgids.h"
#include "picam_app_msg.h"
#include "picam_app_events.h"
#include "picam_app_table.h"


#define PICAM_PIPE_DEPTH                     32 /* Depth of the Command Pipe for Application */

#define PICAM_NUMBER_OF_TABLES               1  /* Number of Table(s) */

/* Define filenames of default data images for tables */
#define PICAM_APP_TABLE_FILE                 "/cf/picam_app_tbl.tbl"

#define PICAM_APP_TABLE_OUT_OF_RANGE_ERR_CODE  -1

#define PICAM_APP_TBL_ELEMENT_1_MAX              10


/*
 * Buffer to hold telemetry data prior to sending
 * Defined as a union to ensure proper alignment for a CFE_SB_Msg_t type
 */
typedef union
{
    CFE_SB_Msg_t        MsgHdr;
    PICAM_HkTlm_t      HkTlm;
} PICAM_HkBuffer_t;

typedef struct
{
    uint8 CmdCounter;
    uint8 ErrCounter;

    PICAM_HkBuffer_t HkBuf;

    uint32 RunStatus;

    CFE_SB_PipeId_t    CommandPipe;
    CFE_SB_MsgPtr_t    MsgPtr;


    char     PipeName[16];
    uint16   PipeDepth;

    CFE_EVS_BinFilter_t  EventFilters[PICAM_EVENT_COUNTS];
    CFE_TBL_Handle_t     TblHandles[PICAM_NUMBER_OF_TABLES];

} PICAM_AppData_t;


void PICAM_AppMain(void);
int32 PICAM_AppInit(void);
int32 PICAM_TblValidationFunc(void *TblData);
void PICAM_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg);
void PICAM_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg);
bool PICAM_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);
int32 PICAM_ReportHousekeeping(const CFE_SB_CmdHdr_t *Msg);

int32 PICAM_Noop(const PICAM_Noop_t *Msg);
int32 PICAM_ResetCounters(const PICAM_ResetCounters_t *Msg);
int32 PICAM_CapturePhoto(const PICAM_Process_t *Msg);

#endif /* _picam_app_h_ */