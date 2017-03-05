/*!
 * @file AlarmClock.h
 * @brief AlarmClock - An Arduino-based RTC-clock with alarm timer and LCD-display
 *
 * @copyright	[cracketus]( https://github.com/cracketus ), 2017
 * @copyright	GNU Lesser General Public License
 *
 * @author [Timur Shevlyakov]
 * @version  V0.1
 * @date  12.02.2017
 */

#ifndef _ALARM_LCD_H_
#define _ALARM_LCD_H_

#include <Arduino.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <InputDebounce.h>

typedef void (*handle_alarm_action)(void);
typedef void (*process_callback)(void);

enum ELcdMode
{
  eDateTime = 0,
  eAlarm = 1,
  eLast = 2
};

class AlarmClock
{  
private:
  LiquidCrystal_I2C m_lcd;
  RTC_DS1307        m_rtc;
  InputDebounce     m_modeButton;
  InputDebounce     m_timeoutButton;
  byte              m_outputPin;

  ELcdMode m_mode;
  bool     m_invalid;
  byte     m_timeout;
  TimeSpan m_alarmTime;

  handle_alarm_action m_alarmAction;

private:
  void updateAlarm();
  void updateDateTime();
  
public:  
  /**
	 * Constructor
	 *
	 * Create AlarmClock object. LCD display initialized as LiquidCrystal_I2C(0x27, 16, 2)
	 */
  AlarmClock() : m_lcd(0x27, 16, 2), m_mode(eDateTime), m_invalid(true), m_timeout(0), m_outputPin(-1), m_alarmAction(nullptr)
  { }
  
  void begin();

  /**
	 * setup
	 *
	 * @param buttonPins	2 supported buttons
	 * @param pressedCallbacks	'pressed' callbacks (see InputDebounce.h library)
	 * @param releasedCallbacks	'released' callbacks (see InputDebounce.h library)
	 * @param pressedDurationCallbacks	'pressed-duration' callbacks (see InputDebounce.h library)
   * @param alarmAction alarm callback
   * @param outputPin set this output pin HIGH level if alarm (-1 - no pin with HIGH level)
	 */
  void setup(byte buttonPins[2],
            inputdebounce_state_cb pressedCallbacks[2],
            inputdebounce_state_cb releasedCallbacks[2],
            inputdebounce_duration_cb pressedDurationCallbacks[2],
            handle_alarm_action alarmAction,
            byte outputPin = -1);

  /**
	 * invalidate

	 * Set invalidate flag for display
	 */
  void invalidate(bool b = true);

  /**
	 * update
   *
	 * Update display output
	 */
  void update();

  /**
	 * processOneSecond
   * 
   * handle events from buttons and timer for 1 second
	 * @param cb callback
	 */
  void processOneSecond(process_callback cb);

  /**
	 * processOneSecond
   * 
   * handle events from buttons
	 */
  void handleButtons();

  /**
	 * processOneSecond
   * 
   * handle alarm event
	 */
  void handleAlarm();

  ELcdMode getMode() { return m_mode; }  
  bool isAlarm();
  bool isAlarmEnabled();
  void nextMode();
  void startTimer();
  void stopTimer();
  void resetTimeout();
  void incTimeout();  

  void setAlarmTime(TimeSpan alarmTime) { m_alarmTime = alarmTime; } ;
  TimeSpan getAlarmTime() { return m_alarmTime; }  
};

#endif
