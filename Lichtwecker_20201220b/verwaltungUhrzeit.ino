void updDispSetTime(int number) { // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
  lcd.setCursor(12,1);
  lcd.print("  ");
  if (number < 10) lcd.setCursor(13,1); else lcd.setCursor(12,1);
  lcd.print(number);
}

void setClock() { // Funktion zum manuellen Einstellen der Uhrzeit vom Bediener
  dispSetHourClock(); // Funktion zum Einstellen der Stunde der aktuellen Uhrzeit am Display
  dispSetMinuteClock(); // Funktion zum Einstellen der Minute der aktuellen Uhrzeit am Display
  setTime(hourClock, minuteClock, 0, 0, 0, 0); // Einstellen der eingestellten Uhrzeit als aktuelle Uhrzeit
}

void dispSetHourClock() { // Funktion zum Einstellen der Stunde der aktuellen Uhrzeit am Display
  lcd.clear();
  lcd.print("Aktuelle Stunde");
  lcd.setCursor(0, 1);
  lcd.print("einstellen:");
  if (hourClock < 10) lcd.setCursor(13,1); else lcd.setCursor(12,1);
  lcd.print(hourClock);

  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
  while (digitalRead(pinButtonMenu) == LOW) { // Ausführen der Einstellung der Zeit, bis die Menü-Taste gedrückt wurde
    currentTime = millis(); // Speichern der aktuellen Systemzeit
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) {
        hourClock++; // Erhöhen der eingestellten Stunde
        if (hourClock > 23) hourClock = 0;
        updDispSetTime(hourClock); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) {
        hourClock--; // Verringern der eingestellten Stunde
        if (hourClock < 0) hourClock = 23;
        updDispSetTime(hourClock); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      }
    }
  }
  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
}

void dispSetMinuteClock() { // Funktion zum Einstellen der Minute der aktuellen Uhrzeit am Display
  lcd.clear();
  lcd.print("Aktuelle Minute");
  lcd.setCursor(0, 1);
  lcd.print("einstellen:");
  if (minuteClock < 10) lcd.setCursor(13,1); 
  else lcd.setCursor(12,1);
  lcd.print(minuteClock);

  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
  while (digitalRead(pinButtonMenu) == LOW) { // Ausführen der Einstellung der Zeit, bis die Menü-Taste gedrückt wurde
    currentTime = millis(); // Speichern der aktuellen Systemzeit
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) {
        minuteClock++; // Erhöhen der eingestellten Minute
        if (minuteClock > 59) minuteClock = 0;
        updDispSetTime(minuteClock); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) {
        minuteClock--; // Verringern der eingestellten Minute
        if (minuteClock < 0) minuteClock = 59;
        updDispSetTime(minuteClock); // Funktion zum Aktualisieren der Zahlenwerte am Display bei Einstellungsmenüs
        lastDebounceTime = currentTime;
      }
    }
  }
  while (digitalRead(pinButtonMenu) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
}
