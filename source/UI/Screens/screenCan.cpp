#include "project.h"
static uint32_t m_eidHeaderText = ELEMENTID_INVALID;
static uint32_t m_eidData2 = ELEMENTID_INVALID;
static void _Key1Release(void* userData);
static void _Key2Release(void* userData);
static void _Key3Release(void* userData);
static void _Key4Release(void* userData);
static void _Key5Release(void* userData);
static char latestMessageText[255] = "Waiting for message...";
static uint64_t rxPackets = 0;
static uint64_t rxBytes = 0;
static CAN_STATS_T m_port1Stats;
static MUTEXHANDLE_T m_canViewerMutex2 = 0;

 
static void _UpdateStatisticsValues(void* pUserData)
{
    (void)pUserData;
    can_statistics_get(CAN_PORT1, &m_port1Stats);
    rxPackets = m_port1Stats.rx_packets;
    rxBytes = m_port1Stats.rx_bytes;
    MMIInvalidate();  // Force a screen redraw
}

void CANViewerLogMessageReceive2(CAN_PORTS_T canPort, CAN_MSG_T* pMsg2)
{
    if (canPort != CAN_PORT1) return;

    MutexLock(&m_canViewerMutex2);
    rxPackets++;
    rxBytes += pMsg2->msg_length;

    strcpy(latestMessageText, "");

    if (pMsg2->extend)
    {
        // Format the J1939 message ID (29-bit)
        sprintf(latestMessageText, "ID: 0x%08X (Ext), Length: %d, Data: ", pMsg2->id, pMsg2->msg_length);
    }
    else
    {
        sprintf(latestMessageText, "ID: 0x%03X (Std), Length: %d, Data: ", pMsg2->id, pMsg2->msg_length);
    }

    for (uint8_t i = 0; i < pMsg2->msg_length; ++i)
    {
        char byteStr[5];
        sprintf(byteStr, "%02X ", pMsg2->msg_content[i]);
        strcat(latestMessageText, byteStr);
    }

    MutexUnlock(&m_canViewerMutex2);
}
void CANViewerInit2(void)
{
    if (m_canViewerMutex2 == 0)
    {
        MutexCreate(&m_canViewerMutex2);
    }
    if (can_open(CAN_PORT1) != 0){
     //   printf("Failed to open CAN_PORT1\n");
    }
    if (can_bitrate_set(CAN_PORT1, 250000) != 0){
   //     printf("Failed to set bitrate for CAN_PORT1\n");
    }

    can_set_receive_callback(CAN_PORT1, CANViewerLogMessageReceive2);
    timer_register(_UpdateStatisticsValues, nullptr, 0, 1000);
}
void ScreenCanEnter(void)
{
    CANViewerInit2();
}
void ScreenCanCreate(void)
{ 
    // Initialize text element IDs
    if (m_eidHeaderText == ELEMENTID_INVALID)
    {
        m_eidHeaderText = ElementGetNewId();
    }
    if (m_eidData2 == ELEMENTID_INVALID)
    {
        m_eidData2 = ElementGetNewId();
    }
    vLcdBlankerEx(WHITE, ALPHA_COLOR);
    ButtonBarSetHeight(48);
    ButtonBarSetKeyText(KEYINDEX_1, FONT_INDEX_TTMAIN, 9, BLACK, "can", "view");
    ButtonBarRegisterKeyReleaseCallback(KEYINDEX_1, _Key1Release, nullptr);
    ButtonBarSetMode(BUTTONBARMODE_VISIBLE_ALWAYS);
   
 
}
void ScreenCanUpdate(void)
{
    ElementFillPrevRect(m_eidData2, WHITE, LAYER_FRONT);
    char str[255];
    SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 10, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);
    sprintf(str, "Rx Packets: %llu", rxPackets);
    SimpleTextDrawEle(m_eidData2, 10, 20, str, BLACK, 100, LAYER_FRONT);

    sprintf(str, "Rx Bytes: %llu", rxBytes);
    SimpleTextDrawEle(m_eidData2, 10, 40, str, BLACK, 100, LAYER_FRONT);

    // Display Latest Message Received
    SimpleTextDrawEle(m_eidData2, 10, 60, latestMessageText, BLACK, 100, LAYER_FRONT);

}
void ScreenCanExit(void)
{
}
static void _Key1Release(void* userData)
{
    //just for checking if the other screen receives can messages
    MMIScreenGoto(SCREENID_CANVIEWER);
}
static void _Key2Release(void* userData)
{
}
static void _Key3Release(void* userData)
{
}
static void _Key4Release(void* userData)
{
}
static void _Key5Release(void* userData)
{
}