#ifndef _picam_app_msg_h_
#define _picam_app_msg_h_

#define PICAM_APP_NOOP_CC                 0
#define PICAM_APP_RESET_COUNTERS_CC       1
#define PICAM_APP_CAPTURE_CC              2


/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} PICAM_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef PICAM_NoArgsCmd_t      PICAM_Noop_t;
typedef PICAM_NoArgsCmd_t      PICAM_ResetCounters_t;
typedef PICAM_NoArgsCmd_t      PICAM_Process_t;

/*************************************************************************/
/*
** Type definition (PICAM App housekeeping)
*/

typedef struct
{
    uint8              CommandErrorCounter;
    uint8              CommandCounter;
    uint8              spare[2];
} PICAM_HkTlm_Payload_t;

typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    PICAM_HkTlm_Payload_t  Payload;

} OS_PACK PICAM_HkTlm_t;

#endif