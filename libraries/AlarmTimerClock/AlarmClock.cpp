/*!
 * @file AlarmClock.cpp
 * @brief AlarmClock - An Arduino-based RTC-clock with alarm timer and LCD-display
 *
 * @copyright	[cracketus]( https://github.com/cracketus ), 2017
 * @copyright	GNU Lesser General Public License
 *
 * @author [Timur Shevlyakov]
 * @version  V0.1
 * @date  12.02.2017
 */

#include "AlarmClock.h"
#include "constants.h"

#include <Arduino.h>
#include <TimeLib.h>

void AlarmClock::begin()
{
  m_rtc.begin();
  if (!m_rtc.isrunning()) 
  {
    m_rtc.adjust(now());
  }
  m_lcd.begin();
  m_lcd.createChar(0, CLOCK_CHAR);
}

void AlarmClock::setup(byte buttonPins[2],
                      inputdebounce_state_cb pressedCallbacks[2],
                      inputdebounce_state_cb releasedCallbacks[2],
                      inputdebounce_duration_cb pressedDurationCallbacks[2],
                      handle_alarm_action alarmAction,
                      byte outputPin)
{
  m_lcd.backlight();
  m_lcd.clear();
  m_lcd.setCursor(0,0);
  
  updateDateTime();
  
  m_modeButton.registerCallbacks(pressedCallbacks[0], releasedCallbacks[0], pressedDurationCallbacks[0]);
  m_timeoutButton.registerCallbacks(pressedCallbacks[1], releasedCallbacks[1], pressedDurationCallbacks[1]);

  m_modeButton.setup(buttonPins[0], BUTTON_DEBOUNCE_DELAY, InputDebounce::PIM_INT_PULL_UP_RES);
  m_timeoutButton.setup(buttonPins[1], BUTTON_DEBOUNCE_DELAY, InputDebounce::PIM_INT_PULL_UP_RES);

  m_alarmAction = alarmAction;

  m_outputPin = outputPin;
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);
}

void AlarmClock::invalidate(bool b)
{
  m_invalid = b;
}

void AlarmClock::update()
{
  if (m_invalid)
  {
    switch (m_mode)
    {
      case eDateTime:
        updateDateTime();
        break;
      case eAlarm:
        updateAlarm();
        break;
      default:
        break;
    }

    m_invalid = false;
  }
}

void AlarmClock::updateAlarm()
{
  m_lcd.clear();
  m_lcd.setCursor(0, 0);

  m_lcd.print("Alarm in ");
  m_lcd.print(m_timeout);
  m_lcd.print(" min.");
}

void AlarmClock::updateDateTime()
{
  DateTime now = m_rtc.now();

  m_lcd.clear();
  m_lcd.setCursor(0, 0);  

  m_lcd.print(now.day(), DEC);
  m_lcd.print('.');
  m_lcd.print(now.month(), DEC);
  m_lcd.print('.');
  m_lcd.print(now.year(), DEC);
  m_lcd.setCursor(0, 1);
  m_lcd.print(now.hour(), DEC);
  m_lcd.print(':');
  m_lcd.print(now.minute(), DEC);
  m_lcd.print(':');
  m_lcd.print(now.second(), DEC);  

  if (m_timeout)
  {
    m_lcd.setCursor(15,1);
    m_lcd.write(0);
  }
}

void AlarmClock::nextMode()
{
  m_mode = static_cast<ELcdMode>(m_mode + 1);
  if (m_mode == eLast)
  {
    m_mode = static_cast<ELcdMode>(0);
  }
}

void AlarmClock::startTimer()
{
  m_alarmTime = m_rtc.now().secondstime();
  m_alarmTime = m_alarmTime + 60 * m_timeout;
}

void AlarmClock::stopTimer()
{
  m_alarmTime = 0;
}

void AlarmClock::resetTimeout()
{
  m_timeout = 0;
}

void AlarmClock::incTimeout()
{
  m_timeout += TIMEOUT_STEP;
  if (m_timeout >= 60)
  {
    m_timeout = 0;
  }
}

void AlarmClock::handleButtons()
{
  unsigned long now = millis();
  m_modeButton.process(now);
  now = millis();
  m_timeoutButton.process(now);
}

bool AlarmClock::isAlarm()
{
  return (m_alarmTime.totalseconds() > 0) && (m_rtc.now().secondstime() >= m_alarmTime.totalseconds());
}

bool AlarmClock::isAlarmEnabled()
{
  return m_timeout > 0;
}

void AlarmClock::handleAlarm()
{
  if (isAlarm())
  {
    if (m_outputPin != -1)
    {
      digitalWrite(m_outputPin, HIGH);
    }
    if (m_alarmAction != nullptr)
    {
      m_alarmAction();
    }
    startTimer();
  }
  else
  {
    if (m_outputPin != -1)
    {
      digitalWrite(m_outputPin, LOW);
    }
  }  
}

void AlarmClock::processOneSecond(process_callback cb)
{
  for (byte i = 0; i < 100; ++i)
  {
    if (cb != nullptr)
    {
      *cb;
    }
    update();
    handleButtons();
    delay(10); 
  }

  if (m_mode == eDateTime)
  {
    invalidate();
  }
}