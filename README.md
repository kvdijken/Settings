# Settings

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

The library builds an internal structure of settings and their allowed values. Building the internal structure is done by adding settings with calling createSetting(). A user program can trigger the editing of the values from a button or a rotary encoder. This can be done by calling settingsDisplayOn() and subsequently settingsUp() and settingsDown() to select the required setting. settingsUp() and settingsDown() are usually called from the main program on detection of a button push or a rotary encoder event. A call to settingsOK() will set the setting into edit mode. Then, settingsUp() and settingsDown() will change the value for the setting. A blue value indicates the current value for the setting, a red value is a value different from the current setting. If during setup of the library the parameter liveUpdate was true, during editing the callback function will be called to reflect the current new value. If liveUpdate = false, the new value will only be transmitted upon user acceptance of the new value by calling settingsOK().

Values for the settings are stored as strings. As such, numerical values, as well as boolean or text values can be used.

Example code:
initialisation for the global variables:

```
Setting *settingCarrierTaps = NULL;
char *valuesCarrierTaps[] = { "50", "100", "150", "200" };
#define N_CARRIER_TAPS 4
#define DEF_CARRIER_TAPS_IDX 0
int tapsCarrierFilter = atoi( valuesCarrierTaps[DEF_CARRIER_TAPS_IDX] );

Setting *settingIF = NULL;
char *valuesIF[] = { "0", "4500", "5000", "7500", "10000", "11000" };
#define N_IFS 6
#define DEF_IF_IDX 1
long iFreq = atoi( valuesIF[DEF_IF_IDX] ) * 100;

```

... and the one time initialisation for the settings library:

```
  // Intermediate frequency
  settingIF = createSetting( "Intermed Freq", valuesIF, N_IFS, DEF_IF_IDX, true, ifChanged );
  result &= (settingIF != NULL);
  // carrier filter taps
  settingCarrierTaps = createSetting( "Carrier FTaps", valuesCarrierTaps, N_CARRIER_TAPS, DEF_CARRIER_TAPS_IDX, true, carrierFilterTapsChanged );
  result = result && (settingCarrierTaps != NULL);

  // Empty line
  createSetting( NULL, NULL, 0, 0, false, NULL );
```

and an example of the event handler when a settings has been changed:

```
bool carrierFilterTapsChanged( Setting *setting ) {
  bool result = true;
  char *end;
  long newValue = strtol( setting->values[setting->newValue], &end, 10 );
  if ( !*end ) {
    result = result && sound( false );
    result = result && setCarrierFilters( carrierFilter.low, carrierFilter.high, newValue );
    result = result && setLagFilters( 500, 9500, newValue );
    result = result && sound( true );
  }
  else
    result = false;
  return result;
}
```