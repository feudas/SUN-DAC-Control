# SUN-DAC-Control
Steuerung des SUN-1000 und Sun-2000 Inverters über eine DAC Steuerung.
Vorweg sei gesagt das die eigentliche Idee mit einem DAC zu steuern von mir nur aufgegriffen wurde. 
Die Quelle der DAC Idee ist von einem User namens Torakas. Im IAMMETER Forum. Der dort
Diesen vorschlag angebracht hatte um an einem Gewinnspiel teilzunehmen.
Solle sonst noch jemand Teile seiner Ideen hier finden und möchte Namendlich 
genannt werden bitte ich um eine entsprechende Mitteilung.

Ich übernehme keine Haftung für Schäden die aufgrund der Anwendung der in dieser
Veröffentlichung genannten Informationen entstehen. 
Der Nutzer verlässt sich allein auf eigenes Risiko und eigene Verantwortung 
auf die von mir gemachten Angaben in dieser Veröffentlichung. 


1. Vorbereitungen am SUN

Die SUN-x000 Inverter sind über einen DAC Eingang steuerbar. Normal ist dieser nicht erreichbar.
Die vorgeschlagene Steuerung kann in den SUN eingebaut werden, es kann aber auch durch umstecken 
des Externen Limiter Anschlusses eine Verbindung des DAC von außen hergestellt werden. 
Da es vielen Menschen leichter fallen wird die externe Anschlussmöglichkeit zu nutzen, beschreibe ich zuerst diese. 
Voraussetzung für diese Erweiterung ist, das Vorhandensein einer RS232 Schnittstelle am SUN Inverter. 
Ohne geht es auch, dann muss aber der 12V Anschluss des Controller Boards zusätzlich nach außen geführt werden. 
Ist die RS232 vorhanden ist alles ok und es können 12V von diesem Anschluss bezogen werden.
Sollte bei einer neueren SUN Version kein EXTERNER Limiter anschluss vorhanden sein,
so kann auch dieser anschluss selbst nach außen geführt werden. 
Der DAC anschluß selbst ist meines wissens nach auch bei den neuen Versionen noch vorhanden.

Der Inverter muss geöffnet werden um Zugang zum Controller Board zu erlangen.
Auf dem Controller Board muss nun der Anschluss vom Externen Limiter umgesteckt werden. 
Er muss von seinem ursprünglichen Anschluss entfernt und auf den Anschluss RT1 gesteckt werden. 
Das ist der Anschluss unten rechts, neben dem roten, auf dem Bild „Control-Board-SUN-1“.

Nun kann der SUN wieder zusammnengebaut werden. 
Es sollte nun der DAC Einnag am Anschluss des EXTERNEN Limiters zur verfügung stehen und ein
12V Anschluss entweder selbst gelegt oder von der RS232 Schnitstelle.


2. DAC Steuerung Programm und Hardware.

Im Ordner "SUN_Steuerung_ESP" findet sich die Programmdatei um den ESP zu Programmieren.
Die Datei ist der einfacheit halber für die Arduinoi API, da diese wohl noch immer am meisten genutzt wird. 
In der Datei müssen noch die Daten für WLAN und MQTT Server angepasst werden.

Achtung, die DAC Steuerung sollte unbedingt über die 12V vom Controller Board versorgt werden, 
um irgendwelchen Schwierigkeiten durch externe Stromversorgungen aus dem Weg zu gehen, 
die bis hin zur Zerstöhrung des SUN Controllerboards gehen können.

- Wir benötigen ein ESP8266 Board. 
  Welches ist eigentlich egal ausser dem I2C und der Versorgungsanschlüsse wird nichts genutzt.
- Einen Spannungsregler 3,3v oder 5v abhängig davon ob euer ESP Board einen 5v Eingang besitzt.
  Ich nutze einen LM2596, da er einstellbar und deshalb für beide Fälle nutzbar ist.
- Einen Widerstand 10kOhm den wir als Pulldown an den DAC Ausgang schalten.

Daraus wird schon klar wie die Schaltung aufgebaut werden muss.
Ein Bild der Schaltung wie ich sie aufgebaut habe, ist auf dem Bild "Circuit.jpg" zu sehen.