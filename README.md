# Centrala za upravljanje pametnim stanom
## Postavljanje i pokretanje
Računalna aplikacija se nalazi u direktoriju `interface`, a Arduino kod sa svim korištenim knjižnicama u `sensor_station` direktoriju.
Za postavljanje Arduina potrebno je učitati sve korištene knjižnice (kopirati iz direktorija `libs` u Arduinov direktorij s knjižnicama, tipično `\Documents\Arduino\Libraries`.
U Arduino kodu baud rate se može postaviti promjenom linije `#define BAUD <željeni baud rate>`.

Nakon uploadanja Arduino koda na pločicu, u `config.ini` datoteci u `interface` direktoriju potrebno je postaviti vrijednosti parametara `port` u COM port na kojeg je spojena pločica i 
`baud` u baud rate definiran u Arduino kodu.

Kada se spoji Arduino i postave vrijednosti u konfiguraciji, aplikacija se može pokrenuti iz direktorija `interface` naredbom

`python3 application.py`

ili iz Python konzole ili razvojnog okruženja.
