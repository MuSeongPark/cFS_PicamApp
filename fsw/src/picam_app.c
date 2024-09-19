

#include <string.h>
#include <stdlib.h>


#include "picam_app.h"
#include "picam_app_version.h"


PICAM_AppData_t PICAM_AppData;

void PICAM_AppMain(void)
{
    int32 status;

    CFE_ES_RegisterApp();

    CFE_ES_PerfLogEntry(PICAM_APP_PERF_ID);

    status = PICAM_AppInit();
    if (status != CFE_SUCCESS)
    {
        PICAM_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }
    
    while (CFE_ES_RunLoop(&PICAM_AppData.RunStatus) == true)
    {
        CFE_ES_PerfLogExit(PICAM_APP_PERF_ID);

        status = CFE_SB_RcvMsg(&PICAM_AppData.MsgPtr, PICAM_AppData.CommandPipe, CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(PICAM_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            PICAM_ProcessCommandPacket(PICAM_AppData.MsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(PICAM_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "PICAM APP: SB Pipe Read Error, App will Exit");
            PICAM_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }
    

    CFE_ES_PerfLogExit(PICAM_APP_PERF_ID);

    CFE_ES_ExitApp(PICAM_AppData.RunStatus);
}

int32 PICAM_AppInit(void)
{
    int32 status;
    PICAM_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;
    
    PICAM_AppData.CmdCounter = 0;
    PICAM_AppData.ErrCounter = 0;

    PICAM_AppData.PipeDepth = PICAM_PIPE_DEPTH;
    strcpy(PICAM_AppData.PipeName, "PICAM_CMD_PIPE");


    status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("PICAM App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    //Initialize housekeeping packet (to send message)
    CFE_SB_InitMsg(&PICAM_AppData.HkBuf.MsgHdr, PICAM_APP_HK_TLM_MID, sizeof(PICAM_AppData.HkBuf), true);

    //Create SW Bus message Pipe (Pipe - to receive message)
    status = CFE_SB_CreatePipe(&PICAM_AppData.CommandPipe, PICAM_AppData.PipeDepth, PICAM_AppData.PipeName);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("PICAM App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    //Subscribe to Housekeeping request commands (to receive Messages)
    status = CFE_SB_Subscribe(PICAM_APP_SEND_HK_MID, PICAM_AppData.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("PICAM App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        
        return status;
    }

    //Subscribe to ground command packets
    status = CFE_SB_Subscribe(PICAM_APP_CMD_MID, PICAM_AppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("PICAM App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    
    //Register Tables
    status = CFE_TBL_Register(&PICAM_AppData.TblHandles[0], "PICAM_AppTable", sizeof(PICAM_APP_Table_t), CFE_TBL_OPT_DEFAULT, PICAM_TblValidationFunc);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("PICAM App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    else
    {
        status = CFE_TBL_Load(PICAM_AppData.TblHandles[0], CFE_TBL_SRC_FILE, PICAM_APP_TABLE_FILE); //

    }

    CFE_EVS_SendEvent(PICAM_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "PICAM App Initialized. %s", PICAM_APP_VERSION_STRING);

    return CFE_SUCCESS;

}

int32 PICAM_TblValidationFunc(void *TblData)
{
    int32 ReturnCode = CFE_SUCCESS;
    PICAM_APP_Table_t *TblDataPtr = (PICAM_APP_Table_t *)TblData;

    if (TblDataPtr->Int1 > PICAM_APP_TBL_ELEMENT_1_MAX)
    {
        ReturnCode = PICAM_APP_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

void PICAM_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg)
{
    CFE_SB_MsgId_t MsgId;
    MsgId = CFE_SB_GetMsgId(Msg);

    switch (MsgId)
    {
        case PICAM_APP_CMD_MID:
            PICAM_ProcessGroundCommand(Msg);
            break;
        
        case PICAM_APP_SEND_HK_MID:
            PICAM_ReportHousekeeping((CFE_SB_CmdHdr_t *) Msg);
            break;

        default:
            CFE_EVS_SendEvent(PICAM_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR, "PICAM: invalid command packet, MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;
}

void PICAM_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(Msg);

    switch (CommandCode)
    {
        case PICAM_APP_NOOP_CC:
            if (PICAM_VerifyCmdLength(Msg, sizeof(PICAM_Noop_t)))
            {
                PICAM_Noop((PICAM_Noop_t *) Msg);
            }

            break;

        case PICAM_APP_RESET_COUNTERS_CC:
            if (PICAM_VerifyCmdLength(Msg, sizeof(PICAM_ResetCounters_t)))
            {
                PICAM_ResetCounters((PICAM_ResetCounters_t *) Msg);

            }
            break;

        case PICAM_APP_CAPTURE_CC:
        //Capture command
            if (PICAM_VerifyCmdLength(Msg, sizeof(PICAM_Process_t)))
            {
                PICAM_CapturePhoto((PICAM_Process_t *) Msg);
            }
            break;
        default:
            CFE_EVS_SendEvent(PICAM_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d", CommandCode);

            break;
    }

    return;
}

bool PICAM_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength)
{
    bool result = true;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(Msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(Msg);

        CFE_EVS_SendEvent(PICAM_LEN_ERR_EID,
            CFE_EVS_EventType_ERROR,
            "Invalid Msg length: ID = 0x%X, CC = %d, Len = %d, Expected = %d",
            (unsigned int)CFE_SB_MsgIdToValue(MessageID),
            CommandCode,
            ActualLength,
            ExpectedLength);
        
        result = false;

        PICAM_AppData.ErrCounter++;

    }

    return result;
}

int32 PICAM_ReportHousekeeping(const CFE_SB_CmdHdr_t *Msg)
{
    int i;

    PICAM_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter = PICAM_AppData.ErrCounter;
    PICAM_AppData.HkBuf.HkTlm.Payload.CommandCounter = PICAM_AppData.CmdCounter;

    CFE_SB_TimeStampMsg(&PICAM_AppData.HkBuf.MsgHdr);
    CFE_SB_SendMsg(&PICAM_AppData.HkBuf.MsgHdr);

    for (i=0; i<PICAM_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(PICAM_AppData.TblHandles[i]);
    }

    return CFE_SUCCESS;

}

int32 PICAM_Noop(const PICAM_Noop_t *Msg)
{
    PICAM_AppData.CmdCounter++;
    CFE_EVS_SendEvent(PICAM_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "PICAM APP: NOOP command %s", PICAM_APP_VERSION);
    return CFE_SUCCESS;
}

int32 PICAM_ResetCounters(const PICAM_ResetCounters_t *Msg)
{
    PICAM_AppData.CmdCounter = 0;
    PICAM_AppData.ErrCounter = 0;

    CFE_EVS_SendEvent(PICAM_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "PICAM APP: RESET command");

    return CFE_SUCCESS;
}

int32 PICAM_CapturePhoto(const PICAM_Process_t *Msg)
{
    PICAM_AppData.CmdCounter++;
    char cmd[100];
    char name[4];
    sprintf(name, "%d", 1);
    strcpy(cmd, "raspistill -n -o ");

    strcat(cmd, "/home/ubuntu/museong/cFS/data/images");
    strcat(cmd, name);
    strcat(cmd, ".jpg &");

    system(cmd);
    CFE_EVS_SendEvent(PICAM_COMMAND_CAPTURE_EID, CFE_EVS_EventType_INFORMATION, "PICAM APP: Capture command");

    return CFE_SUCCESS;
}