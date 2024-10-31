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
	THRUST_MENU,
	ZERO_THRUST,
	TRANSMIT,
	AUTOCALIBRATION
};

Screen_State screen_state = DEFAULT;

// Auto Calibration


void Screen4Enter(void)
{
	
	
}

void Screen4Create(void)
{
	screen_state = DEFAULT;
	vLcdBlankerEx(WHITE, ALPHA_COLOR);

	ScreensSetupDefaultKeys();
	ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
	ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
	ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
	ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
	ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);

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

	} else if (screen_state == THRUST_MENU) {

	} else if (screen_state == TRANSMIT) {

	} else if (screen_state == AUTOCALIBRATION) {
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
	} else if (screen_state == ZERO_THRUST) {

	}

}

void Screen4Exit(void)
{
	autocal_cmd = NONE;
	screen_state = DEFAULT;
}

static void _Key1Release(void* userData)
{
	if (screen_state == DEFAULT) {
		MMIScreenGoto(SCREENID_1);
	} else if (screen_state == THRUST_MENU) {
		MMIScreenGoto(SCREENID_1);
	} else if (screen_state == TRANSMIT) {

	} else if (screen_state == AUTOCALIBRATION) {
		autocal_cmd = FINISH;
		screen_state = DEFAULT;
		ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
		ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	} else if (screen_state == ZERO_THRUST) {
		position_capture_request = 11;
	}
}

static void _Key2Release(void* userData)
{
	if (screen_state == DEFAULT) {
		
	} else if (screen_state == THRUST_MENU) {
		set1_set2_mode = 11;

		screen_state = ZERO_THRUST;
		ButtonBarSetKeyImages(KEYINDEX_1, &set_zthrust, &set_zthrust);
		ButtonBarSetKeyImages(KEYINDEX_2, &finish_calib, &finish_calib);
		ButtonBarSetKeyImages(KEYINDEX_3, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_4, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	} else if (screen_state == TRANSMIT) {
		set1_set2_mode = 0;

		screen_state = DEFAULT;
		ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
		ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	} else if (screen_state == AUTOCALIBRATION) {
		autocal_cmd = ABORT;
		screen_state = DEFAULT;
		ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
		ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	} else if (screen_state == ZERO_THRUST) {
		set1_set2_mode = 0;

		screen_state = DEFAULT;
		ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
		ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	}
}

static void _Key3Release(void* userData)
{
	if (screen_state == DEFAULT) {
		screen_state = THRUST_MENU;
		ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
		ButtonBarSetKeyImages(KEYINDEX_2, &init_zthrust, &init_zthrust);
		ButtonBarSetKeyImages(KEYINDEX_3, &init_transmit, &init_transmit);
		ButtonBarSetKeyImages(KEYINDEX_4, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_5, &exit_menu, &exit_menu);
	} else if (screen_state == THRUST_MENU) {
		set1_set2_mode = 22;

		screen_state = TRANSMIT;
		ButtonBarSetKeyImages(KEYINDEX_1, &set_transmit, &set_transmit);
		ButtonBarSetKeyImages(KEYINDEX_2, &finish_calib, &finish_calib);
		ButtonBarSetKeyImages(KEYINDEX_3, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_4, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	} else if (screen_state == TRANSMIT) {

	} else if (screen_state == AUTOCALIBRATION) {
		autocal_cmd = ABORT;
	} else if (screen_state == ZERO_THRUST) {

	}
}

static void _Key4Release(void* userData)
{
	if (screen_state == DEFAULT) {
		screen_state = AUTOCALIBRATION;
		ButtonBarSetKeyImages(KEYINDEX_1, &finish_calib, &finish_calib);
		ButtonBarSetKeyImages(KEYINDEX_2, &abort_calib, &abort_calib);
		ButtonBarSetKeyImages(KEYINDEX_3, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_4, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_5, &exit_menu, &blank);
		autocal_cmd = INITIALIZE;
	} else if (screen_state == THRUST_MENU) {
		
	} else if (screen_state == TRANSMIT) {

	} else if (screen_state == AUTOCALIBRATION) {

	} else if (screen_state == ZERO_THRUST) {

	}
}

static void _Key5Release(void* userData)
{
	if (screen_state == DEFAULT) {

	} else if (screen_state == THRUST_MENU) {
		screen_state = DEFAULT;
		ButtonBarSetKeyImages(KEYINDEX_1, &view_meters, &view_meters);
		ButtonBarSetKeyImages(KEYINDEX_2, &blank, &blank);
		ButtonBarSetKeyImages(KEYINDEX_3, &thrust_calib, &thrust_calib);
		ButtonBarSetKeyImages(KEYINDEX_4, &init_cal, &init_cal);
		ButtonBarSetKeyImages(KEYINDEX_5, &blank, &blank);
	} else if (screen_state == TRANSMIT) {

	} else if (screen_state == AUTOCALIBRATION) {

	} else if (screen_state == ZERO_THRUST) {

	}
}