#include "project.h"
static uint32_t m_eidHeaderText = ELEMENTID_INVALID;
static uint32_t m_eidData2 = ELEMENTID_INVALID;
//#define MAX_MESSAGE_DISPLAY_LENGTH 100
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
static uint64_t callbackInvocationCount = 0;

// Function to update statistics and check for errors
static void _UpdateStatisticsValues(void* pUserData)
{
    (void)pUserData;
    can_statistics_get(CAN_PORT1, &m_port1Stats);
    rxPackets = m_port1Stats.rx_packets;
    rxBytes = m_port1Stats.rx_bytes;

    // Check CAN port status
    CAN_STATUS_T status = can_status_get(CAN_PORT1);
    if (status != CAN_STATUS_ERROR_ACTIVE)
    {
        // Print or handle the error status
        printf("CAN port status: %d\n", status);
    }

    MMIInvalidate();  // Force a screen redraw
} 


// Define the detailed callback function
void CANViewerDetailedCallback(CALLBACK_INFO_T callbackInfo, CAN_MSG_T* pMsg2)
{
    if (callbackInfo.canPort != CAN_PORT1) return;

    MutexLock(&m_canViewerMutex2);
    rxPackets++;
    rxBytes += pMsg2->msg_length;
    callbackInvocationCount++;

    // Process the message similar to previous code
    // Clear previous message text
    latestMessageText[0] = '\0';

    // Check if the message is in extended format (J1939)
    if (pMsg2->extend)
    {
        snprintf(latestMessageText, sizeof(latestMessageText), "ID: 0x%08X (Ext), Len: %d, Data: ", pMsg2->id, pMsg2->msg_length);
    }
    else
    {
        snprintf(latestMessageText, sizeof(latestMessageText), "ID: 0x%03X (Std), Len: %d, Data: ", pMsg2->id, pMsg2->msg_length);
    }

    // Append the message data safely
    for (uint8_t i = 0; i < pMsg2->msg_length; ++i)
    {
        char byteStr[4];
        snprintf(byteStr, sizeof(byteStr), "%02X ", pMsg2->msg_content[i]);
        strncat(latestMessageText, byteStr, sizeof(latestMessageText) - strlen(latestMessageText) - 1);
    }

    MutexUnlock(&m_canViewerMutex2);
}
 
void CANViewerInit2(void)
{
    if (m_canViewerMutex2 == 0)
    {
        MutexCreate(&m_canViewerMutex2);
    }

    if (can_open(CAN_PORT1) != 0)
    {
        printf("Failed to open CAN_PORT1\n");
        return;
    }

    if (can_bitrate_set(CAN_PORT1, 250000) != 0)
    {
        printf("Failed to set bitrate for CAN_PORT1\n");
        return;
    }

    // Set maximum number of callbacks if needed
    if (!can_set_max_number_callbacks(4))
    {
        printf("Failed to set max number of CAN callbacks\n");
    }

    // Define callback info
    CALLBACK_INFO_T callbackInfo = {
        .canPort = CAN_PORT1,
        .frameDirection = FRAME_DIRECTION_RX,
        .pUserdata = NULL,
        .pLegacyCallback = NULL
    };

    // Register the detailed callback
    if (!can_register_callback(callbackInfo, CANViewerDetailedCallback))
    {
        printf("Failed to register CAN receive callback\n");
    }

    timer_register(_UpdateStatisticsValues, nullptr, 0, 1000);
}
 
void ScreenCanEnter(void)
{
    CANViewerInit2();
}

void ScreenCanCreate(void)
{
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

    // Display Rx Packets
    SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 10, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);
    snprintf(str, sizeof(str), "Rx Packets: %llu", rxPackets);
    SimpleTextDrawEle(m_eidData2, 10, 20, str, BLACK, 100, LAYER_FRONT);

    // Display Rx Bytes
    snprintf(str, sizeof(str), "Rx Bytes: %llu", rxBytes);
    SimpleTextDrawEle(m_eidData2, 10, 40, str, BLACK, 100, LAYER_FRONT);

    // Display Latest Message Received
    SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 10, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);
    snprintf(str, sizeof(str), "Latest Msg:");
    SimpleTextDrawEle(m_eidData2, 10, 60, str, BLACK, 100, LAYER_FRONT);

    // Lock the mutex before accessing latestMessageText
    MutexLock(&m_canViewerMutex2);
    SimpleTextDrawEle(m_eidData2, 10, 80, latestMessageText, BLACK, 100, LAYER_FRONT);
    MutexUnlock(&m_canViewerMutex2);
    // Display Callback Invocation Count
    snprintf(str, sizeof(str), "Callback Count: %llu", callbackInvocationCount);
    SimpleTextDrawEle(m_eidData2, 10, 100, str, BLACK, 100, LAYER_FRONT);
}
void ScreenCanExit(void)
{
	can_close(CAN_PORT1);
	MutexDestroy(&m_canViewerMutex2);
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