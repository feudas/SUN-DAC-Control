# SUN-DAC-Control
Steuerung des SUN-1000 und Sun-2000 Inverters über eine DAC Steuerung.
Vorweg sei gesagt das die eigentliche Idee mit einem DAC zu steuern von mir nur aufgegriffen wurde. 
Ich versuche den Ursprung so gut es geht zu Recherchieren und werde ihn als Quelle noch nachtragen.

Die SUN-x000 Inverter sind über einen DAC Eingang steuerbar. Normal ist dieser nicht erreichbar.
Die vorgeschlagene Steuerung kann in den SUN eingebaut werden, es kann aber auch durch umstecken 
des Externen Limiter Anschlusses eine Verbindung des DAC von außen hergestellt werden. 
Da es vielen Menschen leichter fallen wird die externe Anschlussmöglichkeit zu nutzen, beschreibe ich zuerst diese. 
Voraussetzung für diese Erweiterung ist, das Vorhandensein einer RS232 Schnittstelle am SUN Inverter. 
Ohne geht es auch dann muss aber der 12V Anschluss des Controller Boards zusätzlich nach außen geführt werden. 
Ist die RS232 vorhanden ist alles ok und es können 12V von diesem Anschluss bezogen werden.

Der Inverter muss geöffnet werden um Zugang zum Controller Board zu erlangen.
Auf dem Controller Board muss nun der Anschluss vom Externen Limiter umgesteckt werden. 
Er muss von seinem ursprünglichen Anschluss entfernt und auf den Anschluss RT1 gesteckt werden. 
Das ist der Anschluss unten rechts, neben dem roten, auf dem Bild „Control-Board-SUN-1“.

Nun kann der SUN wieder zusammnengebaut werden. 
Es sollte nun der DAC Einnag am Anschluss des EXTERNEN Limiters zur verfügung stehen und ein
12V Anschluss entweder selbst gelegt oder von der RS232 Schnitstelle.