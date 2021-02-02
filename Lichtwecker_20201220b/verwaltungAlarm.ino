void setAlarm() { // Funktion zum Einstellen der Weckzeit
  lcd.clear();
  dispSetHourAlarm(); // Funktion zum Einstellen der Stunde der Alarmzeit am Display
  dispSetMinuteAlarm(); // Funktion zum Einstellen der Minute der Alarmzeit am Display
  activateAlarm(); // Funktion zum Aktivieren des Alarms auf die zuletzt eingestellte Alarmzeit
}

void dispSetHourAlarm() { // Funktion zum Einstellen der Stunde der Alarmzeit am Display
  lcd.clear();
  lcd.print("Stunde Wecker");
  lcd.setCursor(0, 1);
  lcd.print("einstellen:");
  if (hourAlarm < 10) lcd.setCursor(13,1); else lcd.setCursor(12,1);
  lcd.print(hourAlarm);

  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
  while (digitalRead(pinButtonMenu) == LOW) { // Ausführen der Einstellung der Zeit, bis die Menü-Taste gedrückt wurde
    currentTime = millis(); // Speichern der aktuellen Systemzeit
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) { // Prüfen, ob die Hoch-Taste gedrückt wurde
        hourAlarm++; // Erhöhen der eingestellten Stunde
        if (hourAlarm > 23) hourAlarm = 0;
        updDispSetTime(hourAlarm); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) { // Prüfen, ob die Runter-Taste gedrückt wurde
        hourAlarm--; // Verringern der eingestellten Stunde
        if (hourAlarm < 0) hourAlarm = 23; 
        updDispSetTime(hourAlarm); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      }
    }
  }
  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
}

void dispSetMinuteAlarm() { // Funktion zum Einstellen der Minute der Alarmzeit am Display
  lcd.clear();
  lcd.print("Minute Wecker");
  lcd.setCursor(0, 1);
  lcd.print("einstellen:");
  if (minuteAlarm < 10) lcd.setCursor(13,1); 
  else lcd.setCursor(12,1);
  lcd.print(minuteAlarm);

  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
  while (digitalRead(pinButtonMenu) == LOW) { // Ausführen der Einstellung der Zeit, bis die Menü-Taste gedrückt wurde
    currentTime = millis(); // Speichern der aktuellen Systemzeit
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) {
        minuteAlarm++; // Erhöhen der eingestellten Minute
        if (minuteAlarm > 59) minuteAlarm = 0;
        updDispSetTime(minuteAlarm); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) {
        minuteAlarm--; // Verringern der eingestellten Minute
        if (minuteAlarm < 0) minuteAlarm = 59;
        updDispSetTime(minuteAlarm); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      }
    }
  }
  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
}

void activateAlarm() { // Funktion zum Aktivieren des Alarms auf die zuletzt eingestellte Alarmzeit
  lcd.clear();
  alarmActive = true; // Aktivieren des Weckers
  setStartTimeLight(); // Funktion zum Festsetzen der Startzeit des Lichtweckers
  dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
  dispShowAlarm(); // Funktion zum Anzeigen der Weckzeit auf dem Display
}

void deactivateAlarm() { // Funktion zum Deaktivieren des Alarms
  lcd.clear();
  alarmActive = false; // Deaktivieren des Weckers
  alarmLightActivated = false; // Deaktivieren des Lichtweckers
  menuChange = true; 
  analogWrite(pinLED, 0); // Ausschalten der LED
  dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
}

void ringAlarm() { // Funktion zum Aktivieren des Wecktons
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Alarm!");
  alarmRingDeactivate = false;
  EIFR = bit (INTF1); // Nicht entprelleter Taster verwendet, deswegen alle weiteren versehentlichen Interruptaufrufe löschen.
  attachInterrupt(digitalPinToInterrupt (3), snooze, FALLING); // Interrupt definieren zum Aktivieren der Schlummerfunktion auf der Menü-Taste
  EIFR = bit (INTF0); // Nicht entprelleter Taster verwendet, deswegen alle weiteren versehentlichen Interruptaufrufe löschen.
  attachInterrupt(digitalPinToInterrupt (2), stopAlarm, FALLING); // Interrupt definieren zum Beenden des Alarms auf der Alarm-Taste
  while (alarmRingDeactivate == false) { // Wecker klingeln, bis die Alarm- oder Menü-Taste gedrückt wurde
    tone(pinPiezo, 523, 500); // Einstellen des Piezo-Tons
    delay(800);
  }
  detachInterrupt(digitalPinToInterrupt(3)); // Interrupt schlafen legen
  detachInterrupt(digitalPinToInterrupt(2)); // Interrupt schlafen legen
}

void snooze() { // Funktion zum Aktivieren der Schlummer-Funktion durch Interrupt
  minuteAlarm += snoozeTime; // Neue Weckzeit durch Aufaddierung der Schlummerzeit definieren
  if (minuteAlarm >= 60) {
    hourAlarm++;
    minuteAlarm -= 60;
    if (hourAlarm >= 24) {
      hourAlarm -= 24;
    }
  }
  alarmRingDeactivate = true;
  dispShowTime(); // Funktion zum Anzeigen der aktuellen Uhrzeit auf dem Display
  dispShowAlarm(); // Funktion zum Anzeigen der Weckzeit auf dem Display
  EIFR = bit (INTF1); // Nicht entprelleter Taster verwendet, deswegen alle weiteren versehentlichen Interruptaufrufe löschen.
}

void stopAlarm() { // Funktion zum Deaktivieren des Weckers durch Interrupt
  alarmRingDeactivate = true;
  deactivateAlarm(); // Funktion zum Deaktivieren des Alarms
  EIFR = bit (INTF0); // Nicht entprelleter Taster verwendet, deswegen alle weiteren versehentlichen Interruptaufrufe löschen.
}

void setSnoozeTime() { // Funktion zum Einstellen der Schlummerzeit
  lcd.clear();
  lcd.print("Schlummerzeit");
  lcd.setCursor(0, 1);
  lcd.print("einstellen:");
  if (snoozeTime < 10) lcd.setCursor(13,1); else lcd.setCursor(12,1);
  lcd.print(snoozeTime);

  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
  while (digitalRead(pinButtonMenu) == LOW) { // Ausführen der Einstellung der Zeit, bis die Menü-Taste gedrückt wurde
    currentTime = millis(); // Speichern der aktuellen Systemzeit
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) {
        snoozeTime++; // Erhöhen der eingestellten Minute
        if (snoozeTime > 59) snoozeTime = 0;
        updDispSetTime(snoozeTime); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) {
        snoozeTime--; // Verringern der eingestellten Minute
        if (snoozeTime < 0) snoozeTime = 59;
        updDispSetTime(snoozeTime); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      }
    }
  }
  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
}

void setStartTimeLight() { // Funktion zum Festsetzen der Startzeit des Lichtweckers
  calcTimeTillAlarm(); // Funktion zum Berechnen der Zeit bis zur Weckzeit
  if (hoursTillAlarm == 0 && minutesTillAlarm < leadTimeLightUser) { // Prüfen, ob die Zeit bis zur Weckzeit geringer als die Vorlaufzeit des Lichtweckers ist
    leadTimeLightSystem = minutesTillAlarm - 1; // Überschreiben der Startzeit des Lichtweckers, wenn die Zeit bis zur Alarmzeit geringer als die Vorlaufzeit des Lichtweckers ist.
  } else {
    leadTimeLightSystem = leadTimeLightUser; // Übertragen der durch den Benutzer festgelegten Startzeit des Lichtweckers an die Systemstartzeit
  }
  leadTimeLightSeconds = leadTimeLightSystem * 60; // Speichern der im System festgelegten Vorlaufzeit des Lichtweckers als Sekunden

  // Berechnen der Startzeit des Lichtweckers
  if (minuteAlarm - leadTimeLightSystem < 0) {
    startTimeLightHour = hourAlarm - 1;
    startTimeLightMinute = minuteAlarm - leadTimeLightSystem + 60;
  } else {
    startTimeLightHour = hourAlarm;
    startTimeLightMinute = minuteAlarm - leadTimeLightSystem;
  }
  if (startTimeLightHour < 0) startTimeLightHour += 24;
}

void calcTimeTillAlarm() { // Funktion zum Berechnen der Zeit bis zur Weckzeit
  hoursTillAlarm = hourAlarm - hourClock; // Berechnen der Stunden bis zur Weckzeit

  minutesTillAlarm = minuteAlarm - minuteClock; // Berechnen der Minuten bis zur Weckzeit
  if (minutesTillAlarm < 0) {
    minutesTillAlarm += 60;
    hoursTillAlarm -= 1;
  }
  if (hoursTillAlarm < 0) hoursTillAlarm += 24;
}

void setLeadTimeLight() { // Funktion zum Einstellen der Vorlaufzeit des Lichtweckers durch den Benutzer
  lcd.clear();
  lcd.print("Lichtzeit");
  lcd.setCursor(0, 1);
  lcd.print("einst. Min.:");
  if (leadTimeLightUser < 10) lcd.setCursor(14,1); else lcd.setCursor(13,1);
  lcd.print(leadTimeLightUser);

  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
  while (digitalRead(pinButtonMenu) == LOW) { // Ausführen der Einstellung der Zeit, bis die Menü-Taste gedrückt wurde
    currentTime = millis(); // Speichern der aktuellen Systemzeit
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) {
        leadTimeLightUser++; // Erhöhen der eingestellten Minute
        if (leadTimeLightUser > 59) leadTimeLightUser = 0;
        updDispSetTime(leadTimeLightUser); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) {
        leadTimeLightUser--; // Verringern der eingestellten Minute
        if (leadTimeLightUser < 0) leadTimeLightUser = 59;
        updDispSetTime(leadTimeLightUser); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      }
    }
  }
  leadTimeLightSystem = leadTimeLightUser; // Übertragen der durch den Benutzer festgelegten Startzeit des Lichtweckers an die Systemstartzeit
  leadTimeLightSeconds = leadTimeLightUser * 60; // Speichern der im System festgelegten Vorlaufzeit des Lichtweckers als Sekunden
  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
}
