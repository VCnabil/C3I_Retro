#include "project.h"

static uint32_t m_eidHeaderText = ELEMENTID_INVALID;
static uint32_t m_eidData = ELEMENTID_INVALID;
static void _Key1Release(void* userData);
static void _Key2Release(void* userData);
static void _Key3Release(void* userData);
static void _Key4Release(void* userData);
static void _Key5Release(void* userData);
//char* CalcChecksum(const char* sentence);

static int _cnt = 0;
int saved_value = 0;

void ScreenDebugEnter(void)
{
	//load value from eeprom
	saved_value = SettingsGetTestSave();
	_cnt = saved_value;
}

void ScreenDebugCreate(void)
{
	// Clear layers
	vLcdBlankerEx(WHITE, ALPHA_COLOR);
	// Setup default keys
	ButtonBarSetHeight(48);
	// Setup default keys
	ButtonBarSetKeyText(KEYINDEX_1, FONT_INDEX_TTMAIN, 9, BLACK, "view", "meters");
	ButtonBarSetKeyText(KEYINDEX_2, FONT_INDEX_TTMAIN, 10, BLACK, "CNT", "+");
	ButtonBarSetKeyText(KEYINDEX_3, FONT_INDEX_TTMAIN, 10, BLACK, "CNT", "-");
	ButtonBarSetKeyText(KEYINDEX_4, FONT_INDEX_TTMAIN, 10, BLACK, "CNT", "Save");
	ButtonBarSetKeyText(KEYINDEX_5, FONT_INDEX_TTMAIN, 9, BLACK, "to", "can");

	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_1, _Key1Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_2, _Key2Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_3, _Key3Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_4, _Key4Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_5, _Key5Release, nullptr);

	SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 25, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
	SimpleTextDraw(lcd_get_width() / 2, 5, "EEProm Save", BLACK, 100, LAYER_BACK);

	ButtonBarSetMode(BUTTONBARMODE_VISIBLE_ALWAYS);
}

void ScreenDebugUpdate(void)
{

	fill_lcd_screen(WHITE, LAYER_FRONT);
	SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 25, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
	SimpleTextDraw(lcd_get_width() / 2, lcd_get_height() / 2, "CNT: ", BLACK, 100, LAYER_FRONT);
	SimpleTextDraw(lcd_get_width() / 2, lcd_get_height() / 2 + 50, std::to_string(_cnt).c_str(), BLACK, 100, LAYER_FRONT);


	char str[255] = { 0 };
	MessageQueueInfo_t queueInfo;

	GetQueueInfo(&queueInfo);

	// Clear Previous Header text and redraw
	ElementFillPrevRect(m_eidHeaderText, WHITE, LAYER_FRONT);
	SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 10, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);
	sprintf(str, "UART PORT - RdIndex=%d, WrIndex=%d msgs:%d", queueInfo.NextReadIndex, queueInfo.NextWriteIndex, 3);
	SimpleTextDrawEle(m_eidHeaderText, 5, MenuTitleBarGetHeight() +40, str, BLACK, 100, LAYER_FRONT);

	ElementFillPrevRect(m_eidData, WHITE, LAYER_FRONT);
	SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 10, HORIZONTAL_ALIGNMENT_LEFT, VERTICAL_ALIGNMENT_TOP, 1);

	uint8_t* buf = PeekMessage(PeekTail, 1, (uint8_t*)str, sizeof(str));
	SimpleTextDrawEle(m_eidData, 7, MenuTitleBarGetHeight() + 60, str, BLACK, 100, LAYER_FRONT);

}

void ScreenDebugExit(void)
{
}

static void _Key1Release(void* userData)
{
	MMIScreenGoto(SCREENID_1);
}

static void _Key2Release(void* userData)
{
	_cnt++;
}

static void _Key3Release(void* userData)
{
	_cnt--;
}

static void _Key4Release(void* userData)
{
	saved_value = _cnt;
	SettingsSetTestSave(saved_value);
}

static void _Key5Release(void* userData)
{
	MMIScreenGoto(SCREENID_CAN);
}