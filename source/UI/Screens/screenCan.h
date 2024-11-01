//------------------------------------------------------------------------------
//  TITLE :          screenCan.h
//  DESCRIPTION :    ScreenCan  drawing functions.
//------------------------------------------------------------------------------
#ifndef __SCREENCAN_H__
#define __SCREENCAN_H__

#include "project.h"

/// @addtogroup screens
/// @{

/// Called once when the screen is entered.
/// Clears the Screen, sets the Keys for the screen.
void ScreenCanEnter(void);

/// Called to redraw the entire screen.
/// Clears the Screen, sets the Keys for the screen.
void ScreenCanCreate(void);

/// Drawing Function. 
/// Called every frame.
void ScreenCanUpdate(void);

/// Called when exiting a screen.
void ScreenCanExit(void);

/// @} // endgroup screens

#endif // #ifndef __SCREEN3_H__
