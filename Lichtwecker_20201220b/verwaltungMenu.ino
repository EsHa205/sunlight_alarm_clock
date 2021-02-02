void dispShowMenu() { // Funktion zum Anzeigen des Menüs 
  int menuPoint = 0;
  boolean menuChange = true;
  boolean actionMade = false;

  while (actionMade != true) { // Menü anzeigen, bis eine Aktion ausgeführt wurde
    checkAlarm(); // Funktion zum Prüfen der Alarmbedingungen
    if (menuChange == true) { // Displayanzeige ändern, wenn eine Hoch- oder Runter-Taste gedrückt wurde
      switch (menuPoint) { // Anzeigen des aktuellen Menüpunktes
        case 0:
          lcd.clear();
          lcd.print("Uhrzeit");
          lcd.setCursor(0, 1);
          lcd.print("einstellen");
          menuChange = false;
          break;
        case 1:
          lcd.clear();
          lcd.print("Wecker");
          lcd.setCursor(0, 1);
          lcd.print("einstellen");
          menuChange = false;
          break;
        case 2:
          lcd.clear();
          lcd.print("Wecker");
          lcd.setCursor(0, 1);
          lcd.print("einschalten");
          menuChange = false;
          break;
        case 3:
          lcd.clear();
          lcd.print("Wecker");
          lcd.setCursor(0, 1);
          lcd.print("ausschalten");
          menuChange = false;
          break;
        case 4: 
          lcd.clear();
          lcd.print("Schlummerzeit");
          lcd.setCursor(0, 1);
          lcd.print("einstellen");
          menuChange = false;
          break;
        case 5:
          lcd.clear();
          lcd.print("Lichtzeit");
          lcd.setCursor(0, 1);
          lcd.print("einstellen");
          menuChange = false;
          break;
        case 6:
          lcd.clear();
          lcd.print("Menue verlassen");
          menuChange = false;
          break;
      }
    }

    currentTime = millis(); // Speichern der aktuellen Systemzeit
    if (currentTime - lastDebounceTime >= debounceInterval) { // Prüfen, ob die Taste nicht mehrfach innerhalb des Entprellzeitraums gedrückt wurde
      if (digitalRead(pinButtonUp) == HIGH) { 
        menuPoint++; // Erhöhen des Menüpunktes
        menuChange = true; 
        if (menuPoint > 6) menuPoint = 0;
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonDown) == HIGH) {
        menuPoint--;
        menuChange = true;
        if (menuPoint < 0) menuPoint = 6;
        lastDebounceTime = currentTime;
      } else if (digitalRead(pinButtonMenu) == HIGH) { // Auswählen eines Menüpunktes
        switch (menuPoint) {
          case 0:
            setClock(); // Funktion zum manuellen Einstellen der Uhrzeit vom Bediener
            break;
          case 1:
            setAlarm(); // Funktion zum Einstellen der Weckzeit
            break;
          case 2:
            activateAlarm(); // Funktion zum Aktivieren des Alarms auf die zuletzt eingestellte Alarmzeit
            break;
          case 3:
            deactivateAlarm(); // Funktion zum Deaktivieren des Alarms
            break;
          case 4: 
            setSnoozeTime(); // Funktion zum Einstellen der Schlummerzeit
            break;
          case 5:
            setLeadTimeLight(); // Funktion zum Einstellen der Vorlaufzeit des Lichtweckers durch den Benutzer
            break;
          case 6:
            // Verlassen des Menüs ohne eine Aktion
            while (digitalRead(pinButtonAlarm) == HIGH) {} // Warten, bis Taster losgelassen wurde um den Taster zu Entprellen
            break;
        }
        actionMade = true;
        lastDebounceTime = currentTime;
      }
    }
  }
}
