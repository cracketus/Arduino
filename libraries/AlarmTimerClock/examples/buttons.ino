#include <constants.h>
#include <AlarmClock.h>

static const byte SWITCH_LCD_MODE_PIN =   2;
static const byte SET_ALARM_TIMEOUT_PIN = 3;
static const byte ALARM_LED_PIN =         7;

static AlarmClock alarmClock;

void alarmAction()
{
}

void modeButton_pressedCallback()
{
  alarmClock.nextMode();
  if ((alarmClock.getMode() == eDateTime) && alarmClock.isAlarmEnabled())
  {
    alarmClock.startTimer();
  }
  else if (!alarmClock.isAlarmEnabled())
  {
    alarmClock.stopTimer();
  }
  alarmClock.invalidate();
}

void timeoutButton_pressedCallback()
{
  if (alarmClock.getMode() == eAlarm)
  {
    alarmClock.incTimeout();
    alarmClock.invalidate();
  }
}

void timeoutButton_pressedDurationCallback(unsigned long duration)
{
  if ((alarmClock.getMode() == eAlarm) && (duration > BUTTON_DURATION_PRESS))
  {
    alarmClock.resetTimeout();
    alarmClock.stopTimer();
    alarmClock.invalidate();
  }
}

void button_releasedCallback()
{
}

void button_pressedDurationCallback(unsigned long duration)
{
}

void setup()
{
  Serial.begin(9600);
  alarmClock.begin();

  byte buttonPins[] = { SWITCH_LCD_MODE_PIN, SET_ALARM_TIMEOUT_PIN };
  inputdebounce_state_cb pressedCallbacks[] = { modeButton_pressedCallback, timeoutButton_pressedCallback };
  inputdebounce_state_cb releasedCallbacks[] = { button_releasedCallback, button_releasedCallback };
  inputdebounce_duration_cb pressedDurationCallbacks[] = { button_pressedDurationCallback, timeoutButton_pressedDurationCallback };

  alarmClock.setup(buttonPins, pressedCallbacks, releasedCallbacks, pressedDurationCallbacks, alarmAction, ALARM_LED_PIN);
}

void loop()
{
  alarmClock.processOneSecond(nullptr);
  alarmClock.handleAlarm();
}


