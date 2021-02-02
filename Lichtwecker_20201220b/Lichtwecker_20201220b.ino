#include <TimeLib.h>
#include <LiquidCrystal.h>

#define pinButtonMenu 2 // Menü aufrufen / Bestätigen / Auswählen / Schlummern
#define pinButtonAlarm 3 // Alarm einschalten / Alarm ausschalten
#define pinButtonUp 4 // hoch
#define pinButtonDown 5 // runter
#define pinLED 6
#define pinPiezo 7
#define pinLcdD7 8 // Datenleitung 7
#define pinLcdD6 9 // Datenleitung 6
#define pinLcdD5 10 // Datenleitung 5
#define pinLcdD4 11 // Datenleitung 4
#define pinLcdEnable 12 // Enable
#define pinLcdRegSel 13 // Register Select

#define COLS 16 // Anzahl der Spalten
#define ROWS 2 // Anzahl der Zeilen

LiquidCrystal lcd(pinLcdRegSel, pinLcdEnable, pinLcdD4, pinLcdD5, pinLcdD6, pinLcdD7); // LCD-Objekt instanziieren

// Variablendeklaration und -definition
int hourClock = 0;
int minuteClock = 0;
int minuteRefresh = 0;
int snoozeTime = 5;
int leadTimeLightUser = 30;
int leadTimeLightSystem = 30;
int startTimeLightHour = 0;
int startTimeLightMinute = 0;
int brightnessLight = 0;
int debounceInterval = 250;
int hoursTillAlarm = 0;
int minutesTillAlarm = 0;
volatile int hourAlarm = 0;
volatile int minuteAlarm = 0;

boolean alarmActive = false;
boolean alarmLightActivated = true;
volatile boolean alarmRingDeactivate = false;
volatile boolean snoozeActivated = true;

unsigned long lastDebounceTime = 0;
unsigned long currentTime = 0;

float leadTimeLightSeconds = 0.0f;
float nextBrightnessChangeSecond = 0.0f;

// Prototypen dieses Tabs
void checkTimeChange(); // Funktion zum Prüfen einer Zeitaktualisierung
void checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
void dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
void dispShowAlarm(); // Funktion zum Anzeigen der Weckzeit auf dem Display
void readButtonMenu(); // Funktion zum Prüfen, ob die Menü-Taste gedrückt wurde
void readButtonAlarm(); // Funktion zum Prüfen, ob die Alarm-Taste gedrückt wurde

// Prototypen Tab verwaltungAlarm
void setAlarm(); // Funktion zum Einstellen der Weckzeit
void dispSetHourAlarm(); // Funktion zum Einstellen der Stunde der Alarmzeit am Display
void dispSetMinuteAlarm(); // Funktion zum Einstellen der Minute der Alarmzeit am Display
void activateAlarm(); // Funktion zum Aktivieren des Alarms auf die zuletzt eingestellte Alarmzeit
void deactivateAlarm(); // Funktion zum Deaktivieren des Alarms
void ringAlarm(); // Funktion zum Aktivieren des Wecktons
void snooze(); // Funktion zum Aktivieren der Schlummer-Funktion durch Interrupt
void stopAlarm(); // Funktion zum Deaktivieren des Weckers durch Interrupt
void setSnoozeTime(); // Funktion zum Einstellen der Schlummerzeit
void setStartTimeLight(); // Funktion zum Festsetzen der Startzeit des Lichtweckers
void calcTimeTillAlarm(); // Funktion zum Berechnen der Zeit bis zur Weckzeit
void setLeadTimeLight(); // Funktion zum Einstellen der Vorlaufzeit des Lichtweckers durch den Benutzer

// Prototypen Tab verwaltungMenu
void dispShowMenu(); // Funktion zum Anzeigen des Menüs 

// Prototypen Tab verwaltungUhrzeit
void updDispSetTime(int number); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
void setClock(); // Funktion zum manuellen Einstellen der Uhrzeit vom Bediener
void dispSetHourClock(); // Funktion zum Einstellen der Stunde der aktuellen Uhrzeit am Display
void dispSetMinuteClock(); // Funktion zum Einstellen der Minute der aktuellen Uhrzeit am Display

void setup() {
  pinMode(pinButtonMenu, INPUT);
  pinMode(pinButtonAlarm, INPUT);
  pinMode(pinButtonUp, INPUT);
  pinMode(pinButtonDown, INPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinPiezo, OUTPUT);

  lcd.begin(COLS, ROWS); // Anzahl der Spalten und Zeilen festlegen
  lcd.clear();

  setClock(); // Funktion zum manuellen Einstellen der Uhrzeit vom Bediener
  dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
  minuteRefresh = minute(); // Speichern der aktuellen Minute
  leadTimeLightSeconds = leadTimeLightSystem * 60; // Speichern der im System festgelegten Vorlaufzeit des Lichtweckers als Sekunden
}

void loop() {
  currentTime = millis(); // Speichern der aktuellen Millisekunde seit Programmstart
  
  checkTimeChange();

  checkAlarm();  

  readButtonMenu(); // Funktion zum Prüfen, ob die Menü-Taste gedrückt wurde
  readButtonAlarm(); // Funktion zum Prüfen, ob die Alarm-Taste gedrückt wurde
}

void dispShowTime() { // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
 lcd.setCursor(0, 0); 
 lcd.clear(); 
 lcd.print("Uhrzeit:");
 if (hour() < 10) lcd.setCursor(10,0); else lcd.setCursor(9,0);
 lcd.print(hour());
 lcd.setCursor(11, 0);
 lcd.print(":");
 lcd.setCursor(12,0);
 if (minute() < 10) lcd.print("0"); 
 lcd.print(minute());
}

void dispShowAlarm() { // Funktion zum Anzeigen der Weckzeit auf dem Display
 lcd.setCursor(0, 1);
 lcd.print("Alarm:");
 if (hourAlarm < 10) lcd.setCursor(10,1); else lcd.setCursor(9,1);
 lcd.print(hourAlarm);
 lcd.setCursor(11, 1);
 lcd.print(":");
 lcd.setCursor(12,1);
 if (minuteAlarm < 10) lcd.print("0"); 
 lcd.print(minuteAlarm);
}

void checkTimeChange() { // Funktion zum Prüfen einer Zeitaktualisierung
    if (minute() != minuteRefresh) { // Prüfen, ob sich die aktuelle Minute der Uhrzeit geändert hat, damit das Display nicht flackert
    dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
    minuteRefresh = minute(); // Speichern der aktuellen Minute
    if (alarmActive == true) dispShowAlarm(); // Anzeigen der Alarmzeit, wenn der Wecker aktiv ist
  }
}

void checkAlarm() { // Funktion zum Prüfen der Alarmbedingungen
  if (alarmActive == true && startTimeLightHour == hour() && startTimeLightMinute == minute() && alarmLightActivated == false) { // Prüfen, ob die Startzeit des Lichtweckers erreicht wurde
    alarmLightActivated = true; // Aktivieren des Lichtweckers
  }
  
  if (alarmActive == true && alarmLightActivated == true && round(nextBrightnessChangeSecond) == second()) { // Prüfen, ob der Lichtwecker aktiv ist und die Zeit bis zur nächsten Stufe der LED-Helligkeit vergangen ist
    nextBrightnessChangeSecond += leadTimeLightSeconds / 256.0f; // Berechnen der nächsten Sekunde, wann der LED-Helligkeit erhöht werden soll 
    if (nextBrightnessChangeSecond >= 59.5f) nextBrightnessChangeSecond -= 60.0f; // Prüfen, ob die Sekunde größer als 59,5 ist (aufgrund der Aufrundung in der obigen Prüfung)
    analogWrite(pinLED, brightnessLight++); // Erhöhen der LED-Helligkeit
    if (brightnessLight >= 255) brightnessLight = 254; // Halten der LED-Helligkeit, wenn die maximale Helligkeit erreicht wurde
  }
  
  if (alarmActive == true && hourAlarm == hour() && minuteAlarm == minute()) ringAlarm(); // Prüfen, ob die Weckzeit erreicht wurde
}

void readButtonMenu() { // Funktion zum Prüfen, ob die Menü-Taste gedrückt wurde
  currentTime = millis(); // Speichern der aktuellen Systemzeit
  if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
    if (digitalRead(pinButtonMenu) == HIGH) {
      lastDebounceTime = currentTime;
      while (digitalRead(pinButtonAlarm) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
      dispShowMenu(); // Funktion zum Anzeigen des Systemmenüs
      dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
      if (alarmActive == true) dispShowAlarm(); // Anzeigen der Alarmzeit, wenn der Wecker aktiv ist
    }
  }
}

void readButtonAlarm() { // Funktion zum Prüfen, ob die Alarm-Taste gedrückt wurde
  currentTime = millis(); // Speichern der aktuellen Systemzeit
  if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
    if (digitalRead(pinButtonAlarm) == HIGH) {
      lastDebounceTime = currentTime;
      while (digitalRead(pinButtonAlarm) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
      if (alarmActive == true) { // Prüfen, ob der Wecker bislang aktiv war
        deactivateAlarm(); // Funktion zum Deaktivieren des Alarms
      } else {
        activateAlarm(); // Funktion zum Aktivieren des Alarms auf die zuletzt eingestellte Alarmzeit
      }
    } 
  }
}
