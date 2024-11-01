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
static uint64_t messagesProcessed = 0;
static CAN_STATS_T m_port1Stats;
static MUTEXHANDLE_T m_canViewerMutex2 = 0;
#define MAX_MESSAGES 10  

typedef struct {
    uint32_t id;
    uint8_t extend;
    uint8_t dataLen;
    uint8_t data[CAN_CONTENT_MAX_LENGTH];
} CAN_MESSAGE_DISPLAY_T;

static CAN_MESSAGE_DISPLAY_T messageBuffer[MAX_MESSAGES];
static int messageWriteIndex = 0;
static int messageCount = 0;

static void _UpdateStatisticsValues(void* pUserData)
{
    (void)pUserData;

    can_statistics_get(CAN_PORT1, &m_port1Stats);
    rxPackets = m_port1Stats.rx_packets;
    rxBytes = m_port1Stats.rx_bytes;

    CAN_MSG_T msg;
    while (can_receive_message(CAN_PORT1, &msg) == 0)
    {

        MutexLock(&m_canViewerMutex2);
        messagesProcessed++;

        messageBuffer[messageWriteIndex].id = msg.id;
        messageBuffer[messageWriteIndex].extend = msg.extend;
        messageBuffer[messageWriteIndex].dataLen = msg.msg_length;
        memcpy(messageBuffer[messageWriteIndex].data, msg.msg_content, msg.msg_length);

        messageWriteIndex = (messageWriteIndex + 1) % MAX_MESSAGES;
        if (messageCount < MAX_MESSAGES)
        {
            messageCount++;
        }

        MutexUnlock(&m_canViewerMutex2);
    }

    MMIInvalidate();
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

    timer_register(_UpdateStatisticsValues, nullptr, 0, 100);
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
    ButtonBarSetKeyText(KEYINDEX_1, FONT_INDEX_TTMAIN, 9, BLACK, "CAN", "View");
    ButtonBarRegisterKeyReleaseCallback(KEYINDEX_1, _Key1Release, nullptr);
    ButtonBarSetMode(BUTTONBARMODE_VISIBLE_ALWAYS);
}

void ScreenCanUpdate(void)
{
    ElementFillPrevRect(m_eidData2, WHITE, LAYER_FRONT);
    char str[255];

    SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 8, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);
    snprintf(str, sizeof(str), "Rx Packets: %llu", rxPackets);
    SimpleTextDrawEle(m_eidData2, 10, 10, str, BLACK, 100, LAYER_FRONT);

    snprintf(str, sizeof(str), "Rx Bytes: %llu", rxBytes);
    SimpleTextDrawEle(m_eidData2, 10, 25, str, BLACK, 100, LAYER_FRONT);

    snprintf(str, sizeof(str), "Messages Processed: %llu", messagesProcessed);
    SimpleTextDrawEle(m_eidData2, 10, 40, str, BLACK, 100, LAYER_FRONT);

    SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 8, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);
    snprintf(str, sizeof(str), "Recent Messages:");
    SimpleTextDrawEle(m_eidData2, 10, 55, str, BLACK, 100, LAYER_FRONT);

    MutexLock(&m_canViewerMutex2);

    int displayCount = messageCount;
    int index = messageWriteIndex - 1;
    if (index < 0)
    {
        index = MAX_MESSAGES - 1;
    }

    int yPos = 70;
    int messagesDisplayed = 0;

    while (displayCount > 0 && yPos < lcd_get_height() - 20)
    {
        CAN_MESSAGE_DISPLAY_T* pMsgDisplay = &messageBuffer[index];

        char msgStr[255] = { 0 };
  
   /*     if (pMsgDisplay->extend)
        {
            snprintf(msgStr, sizeof(msgStr), "ID: 0x%08X   :", pMsgDisplay->id );
        }*/

        snprintf(msgStr, sizeof(msgStr), "pgn: 0x%08X :", pMsgDisplay->id);
        // Append the message data
        for (uint8_t i = 0; i < pMsgDisplay->dataLen; ++i)
        {
            char byteStr[4];
            snprintf(byteStr, sizeof(byteStr), "%02X ", pMsgDisplay->data[i]);
            strncat(msgStr, byteStr, sizeof(msgStr) - strlen(msgStr) - 1);
        }

        SimpleTextDrawEle(m_eidData2, 10, yPos, msgStr, BLACK, 100, LAYER_FRONT);
        yPos += 12;

        index--;
        if (index < 0)
        {
            index = MAX_MESSAGES - 1;
        }
        displayCount--;
        messagesDisplayed++;
    }

    MutexUnlock(&m_canViewerMutex2);
}

void ScreenCanExit(void)
{

    can_close(CAN_PORT1);

    MutexDestroy(&m_canViewerMutex2);
}

static void _Key1Release(void* userData)
{
    MMIScreenGoto(SCREENID_DEBUG);
}

static void _Key2Release(void* userData) { }
static void _Key3Release(void* userData) { }
static void _Key4Release(void* userData) { }
static void _Key5Release(void* userData) { }