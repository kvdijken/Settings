/*
 * This library allows setting and changing the values of settings in the program.
 * It creates a list of (name, (value1, value2, ...)) pairs. For each setting (name)
 * the user can browse through the list of allowed values. This can be done by calling
 * settingsUp() or settingsDown(). These can be triggered by either up/down buttons
 * or a rotary encoder. When the user accepts a value, settingsOK() must be called.
 * To cancel the editing of settings, call settingsStop(). 
 * 
 * Output is on a ST7735 display.
 * 
 * Developed 2018 by Koen van Dijken
 */


#ifndef _settings_h_
#define _settings_h_

#include <ST7735_t3.h>       // Hardware-specific library for the ST7735 LCD controller


typedef struct Settings {
  char *name;
  char **values;
  int nValues;        // number of values in 'values'
  int currentValue;   // index into 'values'
  int newValue;       // index into 'values'
  void *fPtr;
  bool liveUpdate;
  bool can;
} Setting;

/*
 * Such a function will be called by the library when the value for a setting
 * has been changed. 
 * 
 * Parameters:
 * setting:       The setting for which the value has been changed
 * 
 * Return:
 * true if the value for the setting has been or will be accepted, false if not
 * 
 */
typedef bool (*ChangeSettingFDef) (Setting *setting);

/**
 * Call to initialise the settings library.
 * 
 * Parameters:
 * n:         max number of settings which can be used.
 * tft:       The display which can be used. The display should already 
 *            be initialised.
 */
bool initSettings( int n, ST7735_t3 *tft );

/**
 * createSetting
 * 
 * Will create a new Setting which can be edited by the settings library.
 * 
 * Parameters:
 * text:    The name of setting. To group settings, pass NULL. This 
 *          creates an empty line.
 * values:  Text representations of the posible values of the setting. It it
 *          an array of *char. The number of entries in the values array
 *          is 'nValues'.
 * nValues: The number of values in the 'values' array.
 * currentValue: index of the current value
 * liveUpdate: allow live updating of value during editing
 * setFPtr: A function pointer to a function which will be called by
 *          library when the value for a setting has been changed. Depending
 *          on 'liveUpdate', this will be after acceptance of the value
 *          (by calling settingsOK()) or during scrolling through the
 *          available values (if 'liveUpdate' == true).
 *          
 * Return:
 * The created Setting, or NULL if it could not be created. This could happen
 * when the number of settings is larger than the maximum number of settings
 * given in 'initSettings()'.
 */
Setting *createSetting( char *text, char **values, int nValues, int currentValue, bool liveUpdate, ChangeSettingFDef setFPtr );

/**
 * Call to indicate that the settings library can take over the display.
 */
bool settingsDisplayOn();

/**
 * Call to indicate that the settings library cannot use the display anymore.
 */
bool settingsDisplayOff();
 
/**
 * To indicate that 'up' has been given.
 * 
 * The state machine in this library decides what action to take, change the selected setting,
 * or change the value of the selected setting.
 */
bool settingsUp();

/**
 * To indicate that 'down' has been given.
 * 
 * The state machine in this library decides what action to take, change the selected setting,
 * or change the value of the selected setting.
 */
bool settingsDown();

/**
 * Call to indicate that 'OK' has been given.
 * 
 * Go into edit mode or save the current value into the selected setting.
 */
bool settingsOK();

/**
 * Call to indicate that 'Cancel' has been given.
 * 
 * Changing the values of settings will be stopped.
 */
bool settingsStop();

#endif
