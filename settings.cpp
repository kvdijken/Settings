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



#include "settings.h"
#include <ST7735_t3.h>       // Hardware-specific library for the ST7735 LCD controller
#include <Adafruit_GFX.h>    // Core graphics library
#include "st7735_properties.h"


bool canUseDisplay = false;
ST7735_t3 *myTFT;
int maxSettings = 0;  // The maximum allowed number of settings.
int nSettings = 0;  // The number of Setting's in 'settings'.
Setting *settings = NULL; // the array of Setting's
int currentSetting = 0; // index of the currently selected setting
int topSetting;   // the topmost setting which is currently displayed.
bool editing = false; // the currently selected setting is being edited now



/**
 * createSetting
 * 
 * Will create a new Setting which can be edited by the settings library.
 * 
 * Parameters:
 * text:    The name of setting
 * values:  Text representations of the posible values of the setting. It it
 *          an array of *char. The number of entries in the values array
 *          is 'nValues'.
 * nValues: The number of values in the 'values' array.
 * setFPtr: A function pointer to a function which can be called by
 *          library when the value for a setting has been changed.
 *          
 * Return:
 * The created Setting, or NULL if it could not be created. This could happen
 * when the number of settings is larger than the maximum number of settings
 * given in 'initSettings()'.
 */
Setting *createSetting( char *text, char **values, int nValues, int currentValue, bool liveUpdate, ChangeSettingFDef setFPtr ) {
  Setting *setting = NULL;
  if( nSettings == maxSettings )
    return setting;
  setting = &settings[nSettings];
  setting->name = text;
  setting->values = values;
  setting->nValues = nValues;
  setting->currentValue = currentValue;
  setting->newValue = currentValue;
  setting->fPtr = (void *) setFPtr;
  setting->liveUpdate = liveUpdate;
  nSettings++;
  return setting;
}


/**
 * Call to initialise the settings library.
 * 
 * Parameters:
 * n:         max number of settings which can be used.
 * tft:       The display which can be used. The display should already 
 *            be initialised.
 */
bool initSettings( int n, ST7735_t3 *tft ) {
  bool result = true;
  myTFT = tft;
  maxSettings = n;
  settings = (Setting *) malloc( sizeof( Setting ) * n );
  result = result && (settings != NULL);
  return result;
}


/**
 * 
 */
bool printAt( int x, int y, char *text, bool clean, int colorFG, int colorBG, int leading ) {
  bool result = true;
  if( !canUseDisplay )
    return result;
  int col = x * CHAR_WIDTH;
  int row = y * CHAR_HEIGHT;
  if ( clean )
    myTFT->fillRect( col, row, (strlen( text ) + leading) * CHAR_WIDTH, CHAR_HEIGHT, colorBG );
  if( text != NULL ) {
    myTFT->setCursor( col, row );
    myTFT->setTextColor( colorFG );
    for( int i=0; i<leading; i++ )
      myTFT->print( " " );
    myTFT->print( text );
  }
  return result;
}



/*
012345....0....5....0....5.
> SAMPLERATE       96000
  IF               5000
  INTERMED FREQ
  OUT FILTER MAX
  OUT FILTER TAPS
 */


/**
 * 
 */
bool displayName( int i, int row, bool clean, int colorFG, int colorBG ) {
  bool result = true;
  if( settings == NULL )
    return false;
  result = result && printAt( 2, row, settings[i].name, clean, colorFG, colorBG, 0 );
  return result;
}


/**
 * 
 */
bool displayValue( int i, int row, bool clean, int colorFG, int colorBG ) {
  bool result = true;
  if( settings == NULL )
    return false;
  int actual = settings[i].newValue;
  result = result && printAt( 19, row, settings[i].values[actual], clean, colorFG, colorBG, TFT_CHARS - 19 - strlen(settings[i].values[actual]) );
  return result;
}


/**
 * 
 */
bool highlightValue() {
  bool result = true;
  int row = currentSetting - topSetting;

  // Determine the color to display the value
  // WHITE when not editing
  // BLUE when editing, but (value to display)  == (current value of setting)
  // RED when editing, but (value to display)  != (current value of setting)
  Setting *setting = &settings[currentSetting];
  int color;
  if( editing ) {
    bool valueIsCurrent = setting->currentValue == setting->newValue;
    if( valueIsCurrent )
      color = BLUE;
    else
      color = RED;
  } else
    color = WHITE;
    
  result = result &&  displayValue( currentSetting, row, true, color, BLACK );
  return result;
}


/**
 * 
 */
bool displaySetting( int i, int row, bool clean, int colorFG, int colorBG ) {
  bool result = true;
  if( settings == NULL )
    return false;
  if( settings[i].name != NULL ) {
    result = result && displayName( i, row, clean, colorFG, colorBG );
    result = result && displayValue( i, row, clean, colorFG, colorBG );
  } 
  else
    printAt( 0, row, "                          ", true, WHITE, BLACK, 0 );
  return result;
}


/**
 * 
 */
bool displaySettings( int first ) {
  bool result = true;

  if( !canUseDisplay )
    return false;
    
  // clear the screen
  myTFT->fillScreen( ST7735_BLACK );

  // how many lines to display?
  int n = nSettings - first;
  if( n > TFT_LINES)
    n = TFT_LINES;

  // Display each setting from first to first+n-1
  for( int i=0; i<n && result; i++ )
    result = result && displaySetting( first+i, i, false, WHITE, BLACK );

  topSetting = first;
  
  return result;
}


/**
 * 
 */
bool selectSetting( bool on ) {
  bool result = true;
  int row = currentSetting - topSetting;
  char *sel = ">";
  char *space = " ";
  result = result && printAt( 0, row, on ? sel : space, true, WHITE, BLACK, 0 ); 
  return result;
}


/**
 * Call to indicate that the settings library can take over the display.
 */
bool settingsDisplayOn() {
  bool result = true;
  canUseDisplay = true;
  result = result && displaySettings( topSetting );
  selectSetting( true );
  return result;
}


/**
 * Call to indicate that the settings library cannot use the display anymore.
 */
bool settingsDisplayOff() {
  bool result = true;
  canUseDisplay = false;
  editing = false;  // to prevent confusion
  return result;
}


/**
 * 
 */
bool scrollValue( int d ) {
  bool result = true;
  Setting *setting = &settings[currentSetting];
  if( setting == NULL )
    return false;
  
  // determine the new value to select
  int currentNewValue = setting->newValue;
  int newNewValue = setting->newValue;
  if( d > 0 && currentNewValue < setting->nValues-1 )
    newNewValue += d;
  else
    if( d < 0 && currentNewValue > 0 )
      newNewValue += d;

  // if it is different than the current value, select it
  if( newNewValue != currentNewValue ) {
    setting->newValue = newNewValue;
    result = result && highlightValue();
    if( setting->liveUpdate )
      // save result to be able to reset in settingsOK() if
      // this value is not accepted for some reason.
      setting->can = (((ChangeSettingFDef) setting->fPtr)(setting));
  }

  return result;
}


/**
 * 
 */
bool scrollSetting( int d ) {
  bool result = true;
  // determine the new setting to select
  int newSetting = currentSetting;
  do {
    if( d > 0 && newSetting < nSettings-1 )
      newSetting += d;
    else if( d < 0 && newSetting > 0 )
      newSetting += d;
    else
      return false;
  } while (settings[newSetting].name == NULL );

  // if it is different than the current setting, select it
  if( newSetting != currentSetting ) {
    // unselect the previous current setting ...
    selectSetting( false );
    // ... to select the new current setting
    currentSetting = newSetting;
    // Is the current setting still visible on the screen?
    if( currentSetting < topSetting ) {
      result = result && displaySettings( currentSetting ); 
    }
    else if( currentSetting >= topSetting + TFT_LINES ) {
      result = result && displaySettings( currentSetting - TFT_LINES + 1 ); 
    }
    selectSetting( true );
  }
  return result;
}


/**
 * To indicate that 'up' has been given.
 * 
 * The state machine in this library decides what action to take, change the selected setting,
 * or change the value of the selected setting.
 */
bool settingsUp() {
  bool result = true;
  if( editing ) {
      result = result && scrollValue( 1 );
  } else {
      result = result && scrollSetting( 1 );
  }
  return result;
}


/**
 * To indicate that 'down' has been given.
 * 
 * The state machine in this library decides what action to take, change the selected setting,
 * or change the value of the selected setting.
 */
bool settingsDown() {
  bool result = true;
  if( editing ) {
      result = result && scrollValue( -1 );
  } else {
      result = result && scrollSetting( -1 );
  }
  return result;
}


/**
 * Call to indicate that 'OK' has been given.
 * 
 * The state machine in this library decides what action to take, go into edit mode
 * or save the current value into the selected setting.
 */
bool settingsOK() {
  bool result = true;
  Setting *setting = &settings[currentSetting];
  if( editing ) {
    // change value of setting
    if( setting->newValue != setting->currentValue )
      if( setting->liveUpdate ) {
        // The setting has already been updated to its new value
        if( setting->can )
          setting->currentValue = setting->newValue;
        else       
          setting->newValue = setting->currentValue;
      }
      else if(((ChangeSettingFDef) setting->fPtr)(setting))
        setting->currentValue = setting->newValue;
      else
        setting->newValue = setting->currentValue;
  } else {
    // start editing the value of the current setting
    // ...?
  }
  if( result )
    editing = !editing;
  highlightValue();    
  return result;
}


/**
  // Must the value be reset to its current value? This is 
  // the case when this setting will be updated live AND
  // the new value != current value AND the new value
  // has been accepted by the client.
 */
bool resetNewValue() {
  bool result = true;
  Setting *setting = &settings[currentSetting];

  // Must the value be reset to its current value? This is 
  // the case when this setting will be updated live AND
  // the new value != current value AND the new value
  // has been accepted by the client.
  bool resetLive = setting->liveUpdate && 
                   setting->can &&
                   (setting->newValue != setting->currentValue);
  setting->newValue = setting->currentValue;
  if( resetLive )
    // Not interested in the result of this call.
    (((ChangeSettingFDef) setting->fPtr)(setting));

  return result;
}


/**
 * Call to indicate that 'Cancel' has been given.
 * 
 * 
 */
bool settingsStop() {
  bool result = true;
  if( editing ) {
    result = result && resetNewValue();
  } else {
    //  Nothing to be done here
  }
  return result;
}

