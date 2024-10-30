#include "project.h"

// Private Variables and Functions

//Button Bar
static void _Key1Release(void* userData);
static void _Key2Release(void* userData);
static void _Key3Release(void* userData);
static void _Key4Release(void* userData);
static void _Key5Release(void* userData);

static void Set_Autocal_Flag_INIT(void* userData);
static void Set_Autocal_Flag_END(void* userData);
static void Set_Autocal_Flag_ABORT(void* userData);

enum Screen_State {
	DEFAULT,
	ZERO_THRUST,
	AUTOCALIBRATION
};

Screen_State screen_state = DEFAULT;

// Auto Calibration


void Screen4Enter(void)
{

}

void Screen4Create(void)
{
	vLcdBlankerEx(WHITE, ALPHA_COLOR);

	ScreensSetupDefaultKeys();
	ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
	ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
	ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
	ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
	ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);

	ButtonBarSetMode(BUTTONBARMODE_VISIBLE_ALWAYS);

	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_1, _Key1Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_2, _Key2Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_3, _Key3Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_4, _Key4Release, nullptr);
	ButtonBarRegisterKeyReleaseCallback(KEYINDEX_5, _Key5Release, nullptr);

	SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 25, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
	SimpleTextDraw(lcd_get_width() / 2, 5, "System Calibration", BLACK, 100, LAYER_BACK);

}

void Screen4Update(void)
{
	fill_lcd_screen(WHITE, LAYER_FRONT);
	processSerial();
	sendSerial();
	if (screen_state == DEFAULT) {

	}
	if (screen_state == AUTOCALIBRATION) {
		switch (Autocal_Statusi)
		{
		case 0:
			SimpleTextDraw(160, 50, "Not Active", BLACK, 100, LAYER_FRONT);
			break;

		case 11:
			SimpleTextDraw(160, 50, "ACTIVE", BLACK, 100, LAYER_FRONT);
			SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 18, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
			SimpleTextDraw(160, 80, "Cylinders Moving Up / Stbd", BLACK, 100, LAYER_FRONT);
			break;

		case 22:
			SimpleTextDraw(160, 50, "ACTIVE", BLACK, 100, LAYER_FRONT);
			SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 18, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
			SimpleTextDraw(160, 80, "Cylinders Moving Down / Port", BLACK, 100, LAYER_FRONT);
			break;

		case 33:
			SimpleTextDraw(160, 50, "ACTIVE", BLACK, 100, LAYER_FRONT);
			SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 18, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
			SimpleTextDraw(160, 80, "Cycle Controls...", BLACK, 100, LAYER_FRONT);
			break;

		case 44:
			SimpleTextDraw(160, 50, "COMPLETE", BLACK, 100, LAYER_FRONT);
			SimpleTextSetupFontEx(FONT_INDEX_TTMAIN, 18, HORIZONTAL_ALIGNMENT_CENTRE, VERTICAL_ALIGNMENT_TOP, 0);
			SimpleTextDraw(160, 80, "Move controls to detent", BLACK, 100, LAYER_FRONT);
			break;

		default:
			SimpleTextDraw(190, 50, "Unknown Status", BLACK, 100, LAYER_FRONT);
			break;
		}
	}
	if (screen_state == ZERO_THRUST) {

	}

}

void Screen4Exit(void)
{
	autocal_cmd = NONE;
	screen_state = DEFAULT;
}

static void _Key1Release(void* userData)
{
	MMIScreenGoto(SCREENID_1);
}

static void _Key2Release(void* userData)
{
	if (screen_state == DEFAULT) {

	}
	if (screen_state == AUTOCALIBRATION) {
		autocal_cmd = FINISH;
	}
	if (screen_state == ZERO_THRUST) {

	}
}

static void _Key3Release(void* userData)
{
	if (screen_state == DEFAULT) {
		screen_state = ZERO_THRUST;
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_4, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	}
	if (screen_state == AUTOCALIBRATION) {
		autocal_cmd = ABORT;
	}
	if (screen_state == ZERO_THRUST) {

	}
}

static void _Key4Release(void* userData)
{
	if (screen_state == DEFAULT) {
		screen_state = AUTOCALIBRATION;
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_4, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);

	}
	if (screen_state == AUTOCALIBRATION) {

	}
	if (screen_state == ZERO_THRUST) {

	}
}

static void _Key5Release(void* userData)
{
	if (screen_state == DEFAULT) {

	}
	if (screen_state == AUTOCALIBRATION) {

	}
	if (screen_state == ZERO_THRUST) {

	}
}