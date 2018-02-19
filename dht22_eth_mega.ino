#include <SPI.h>             //библиотека для работы с SPI
#include <UIPEthernet.h>        //библиотека для работы с Ethernet 
#include <dht.h>        //библиотека для работы с DHT22 
#include <IRremote.h>        //библиотека для работы с IR

#define DHT22_PIN 2

//инициализация библиотеки dht
dht DHT;

int dht_status = 0;

float MAX_TEMP = 25.0;
float MIN_TEMP = 24.0;
int last_condey_status = 0;

boolean newInfo = 0;        //переменная для новой информации
//MAC адрес вашего Ethernet-модуля, если его у вас нет, введите любой
//или оставьте тот, что в примере
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };

//указываем IP адрес
//будьте внимательны, если адрес занят другим устройством, то ничего работать не будет
IPAddress ip(x, x, x, x);

//инициализация библиотеки Ethernet server library
EthernetServer server(80);

unsigned int DefaultMode[] = {4620, 4420, 640, 1540, 660, 480, 640, 1560, 620, 1600, 620, 460, 620, 500, 620, 1580, 640, 480, 620, 460, 640, 1600, 620, 460, 620, 480, 640, 1560, 640, 1580, 640, 480, 600, 1600, 620, 480, 600, 520, 600, 480, 620, 1600, 620, 1560, 640, 1600, 600, 1600, 620, 1580, 620, 1600, 620, 1560, 640, 1600, 600, 480, 620, 500, 620, 480, 600, 520, 600, 500, 580, 520, 600, 1600, 600, 1600, 620, 500, 600, 1600, 620, 480, 620, 500, 600, 500, 580, 1620, 620, 480, 600, 520, 580, 1620, 580, 540, 580, 1600, 600, 1620, 600, 1600, 580, 5320, 4540, 4460, 600, 1600, 600, 520, 600, 1580, 600, 1620, 600, 500, 580, 540, 580, 1620, 580, 520, 580, 520, 580, 1640, 580, 520, 560, 540, 580, 1620, 580, 1640, 580, 500, 580, 1640, 580, 520, 600, 520, 560, 540, 560, 1640, 580, 1660, 560, 1620, 560, 1680, 540, 1640, 560, 1660, 560, 1660, 560, 1640, 560, 560, 540, 560, 540, 540, 540, 580, 540, 560, 520, 600, 520, 1660, 540, 1680, 540, 560, 520, 1700, 520, 560, 540, 580, 540, 560, 520, 1700, 540, 540, 540, 580, 540, 1660, 520, 600, 520, 1660, 540, 1680, 540, 1680, 540}; // DefaultOn (21C + Auto)
unsigned int OffMode[] = {4460, 4332, 604, 1532, 608, 492, 604, 1536, 604, 1580, 584, 468, 604, 492, 600, 1556, 584, 516, 580, 468, 604, 1584, 580, 472, 600, 492, 604, 1556, 584, 1564, 600, 492, 576, 1568, 600, 484, 584, 1568, 600, 1560, 580, 1568, 596, 1580, 584, 492, 580, 1568, 596, 1544, 596, 1588, 580, 484, 584, 508, 588, 476, 592, 500, 600, 1540, 596, 504, 592, 492, 580, 1576, 588, 1564, 576, 1576, 588, 496, 576, 520, 572, 492, 580, 520, 576, 492, 576, 520, 576, 496, 576, 492, 604, 1564, 576, 1588, 576, 1564, 576, 1588, 576, 1564, 576, 5168, 4404, 4364, 568, 1572, 572, 524, 568, 1568, 572, 1612, 552, 504, 568, 524, 568, 1592, 548, 548, 548, 516, 552, 1624, 540, 528, 540, 556, 540, 1600, 540, 1624, 540, 532, 536, 1628, 536, 536, 560, 1604, 536, 1632, 532, 1608, 532, 1636, 528, 540, 528, 1640, 524, 1624, 516, 1660, 504, 592, 500, 552, 516, 548, 524, 572, 520, 1624, 516, 576, 520, 552, 520, 1648, 516, 1648, 516, 1628, 512, 576, 524, 548, 520, 548, 520, 580, 516, 552, 516, 580, 516, 552, 520, 576, 520, 1636, 504, 1652, 512, 1624, 520, 1652, 512, 1652, 512}; // Off
unsigned int ComfortMode[] = {4600, 4440, 640, 1560, 620, 500, 620, 1580, 620, 1580, 640, 460, 620, 520, 600, 1580, 620, 500, 620, 460, 620, 1600, 620, 480, 620, 500, 620, 1560, 620, 1600, 620, 500, 600, 1580, 640, 460, 620, 500, 620, 480, 620, 1580, 640, 1560, 640, 1600, 600, 1600, 620, 1580, 620, 1600, 620, 1560, 640, 1600, 620, 460, 620, 500, 620, 480, 600, 500, 620, 480, 620, 500, 620, 1580, 600, 1600, 620, 1600, 600, 520, 580, 1600, 600, 520, 600, 480, 620, 1600, 620, 480, 600, 520, 600, 480, 620, 1600, 620, 480, 600, 1620, 600, 1600, 600, 5280, 4560, 4460, 600, 1600, 600, 520, 580, 1600, 600, 1620, 600, 500, 600, 520, 580, 1600, 600, 520, 600, 500, 580, 1640, 580, 500, 600, 520, 580, 1620, 580, 1640, 580, 500, 600, 1620, 600, 500, 600, 520, 580, 500, 580, 1640, 580, 1640, 580, 1620, 580, 1640, 580, 1620, 560, 1660, 560, 1660, 560, 1620, 580, 540, 580, 500, 580, 520, 580, 540, 560, 540, 560, 560, 560, 1640, 540, 1680, 540, 1680, 540, 560, 520, 1700, 520, 560, 540, 560, 560, 1640, 540, 580, 540, 560, 540, 540, 540, 1700, 520, 560, 540, 1680, 540, 1680, 540}; // 22C + Dry
unsigned int VitalyMode[] = {4600, 4440, 640, 1560, 620, 500, 620, 1560, 640, 1580, 640, 460, 620, 500, 620, 1580, 620, 500, 620, 460, 620, 1600, 620, 480, 600, 500, 620, 1600, 600, 1600, 620, 480, 620, 1600, 620, 460, 620, 500, 620, 1560, 640, 1600, 620, 1580, 640, 1560, 620, 1600, 620, 1580, 620, 1600, 620, 1560, 620, 500, 620, 500, 600, 500, 620, 480, 600, 500, 620, 480, 620, 500, 620, 480, 600, 1600, 620, 1580, 620, 500, 600, 500, 600, 520, 600, 480, 600, 1620, 620, 1560, 620, 520, 600, 480, 600, 1600, 640, 1580, 600, 1620, 600, 1580, 600, 5300, 4540, 4480, 600, 1600, 580, 540, 580, 1600, 600, 1620, 600, 500, 580, 540, 580, 1600, 600, 520, 600, 500, 580, 1640, 580, 500, 580, 540, 580, 1620, 580, 1640, 580, 500, 580, 1640, 580, 520, 600, 500, 600, 1640, 580, 1600, 580, 1640, 580, 1620, 560, 1660, 560, 1640, 560, 1640, 580, 1660, 560, 520, 560, 560, 560, 540, 560, 520, 560, 580, 540, 540, 560, 560, 540, 560, 540, 1680, 540, 1660, 520, 580, 540, 560, 540, 580, 540, 560, 520, 1700, 520, 1660, 540, 580, 540, 540, 540, 1680, 540, 1660, 540, 1680, 540, 1680, 540}; // 19C + Cool + Fan High
unsigned int SwingMode[] = {4260, 4508, 432, 1708, 428, 656, 440, 1712, 428, 1736, 432, 588, 480, 664, 432, 1708, 428, 656, 448, 636, 428, 1736, 428, 640, 432, 656, 440, 1712, 428, 1736, 428, 640, 428, 1740, 428, 640, 428, 1740, 428, 1712, 424, 652, 444, 1720, 448, 620, 448, 1724, 440, 1712, 428, 1720, 448, 624, 444, 672, 424, 1688, 452, 648, 448, 1668, 472, 644, 452, 620, 448, 1692, 476, 1668, 468, 1692, 476, 620, 452, 620, 472, 572, 500, 592, 504, 572, 496, 596, 500, 556, 512, 556, 540, 1684, 456, 1712, 452, 1696, 444, 1720, 444, 1716, 424, 5324, 4252, 4540, 388, 1752, 388, 708, 388, 1760, 380, 1796, 368, 684, 384, 720, 376, 1780, 360, 736, 360, 712, 356, 1832, 332, 716, 356, 740, 352, 1812, 328, 1844, 324, 740, 328, 1856, 308, 748, 348, 1840, 300, 1872, 292, 772, 300, 1628, 540, 508, 560, 1628, 536, 1604, 536, 1628, 536, 520, 548, 556, 540, 1604, 532, 564, 532, 1608, 528, 568, 532, 540, 524, 1644, 520, 1644, 524, 1616, 520, 576, 520, 548, 524, 548, 520, 576, 520, 548, 520, 576, 520, 552, 516, 580, 516, 1640, 500, 1672, 496, 1644, 496, 1660, 504, 1668, 500}; // Swing

int freq = 38;

//инициализация библиотеки irsend
IRsend irsend;

unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 4000;

int set_auto_mark = 0;


void SetAutoMode(float temp, int trigger)
{
  int status = 0;

  if (trigger == 1)
  {
    Serial.println("Trigger is on");

    if (temp > MAX_TEMP) {
      status = 1;
      Serial.print("Condition status STAT1 is: ");
      Serial.println(status);
    }

    if (temp < MIN_TEMP) {
      status = 2;
      Serial.print("Condition status STAT2 is: ");
      Serial.println(status);
    }

    if (status != last_condey_status) {
      last_condey_status = status;
      if (status == 2) {
        irsend.sendRaw(OffMode, sizeof(OffMode) / sizeof(OffMode[0]), freq);
      }
      if (status == 1) {
        irsend.sendRaw(ComfortMode, sizeof(ComfortMode) / sizeof(ComfortMode[0]), freq);
        delay(50);
        irsend.sendRaw(SwingMode, sizeof(SwingMode) / sizeof(SwingMode[0]), freq);
      }
    }
    Serial.print("Condition status is: ");
    Serial.println(status);
  }
  else {
    Serial.println("Trigger is off");
  }
}

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    Serial.println("status,\tHumidity (%),\tTemperature (C)");
    // считываем значения с датчика DHT22
    int chk = DHT.read22(DHT22_PIN);
    switch (chk)
    {
      case DHTLIB_OK:
        dht_status = 200;
        Serial.print("OK,\t");
        break;
      case DHTLIB_ERROR_CHECKSUM:
        dht_status = 501;
        Serial.print("Checksum error,\t");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        dht_status = 504;
        Serial.print("Time out error,\t");
        break;
      default:
        dht_status = 500;
        Serial.print("Unknown error,\t");
        break;
    }
    // отображаем значения с датчика DHT22
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t");
    Serial.println(DHT.temperature, 1);
    previousMillis = currentMillis;
  }

  SetAutoMode(DHT.temperature, set_auto_mark);

  //принимаем данные, посылаемые клиентом
  EthernetClient client = server.available();
  if (client) {                                     //если запрос оканчивается пустой строкой
    boolean currentLineIsBlank = true;                //ставим метку об окончании запроса (дословно: текущая линия чиста)
    while (client.connected()) {                      //пока есть соединение с клиентом
      if (client.available()) {                       //если клиент активен
        char c = client.read();                       //считываем посылаемую информацию в переменную "с"

        if (newInfo && c == ' ') {                    //если переменная новой информации = 1 и "с", в которой записан запрос, равен пустой строке
          newInfo = 0;                                //то обнуляем переменную поступления новой информации
        }

        if (c == '$') {                               //если переменная "с", несущая отправленный нам запрос, содержит символ $
          //(все новые запросы) - "$" подразумевает разделение получаемой информации (символов)
          newInfo = 1;                                //то пришла новая информация, ставим метку новой информации в 1
        }

        //Проверяем содержание URL - присутствует $1 или $2
        if (newInfo == 1) {                            //если есть новая информация
          Serial.println(c);
          if (c == '1') {                            //и "с" содержит 1
            Serial.println("ON");
            irsend.sendRaw(ComfortMode, sizeof(ComfortMode) / sizeof(ComfortMode[0]), freq);
            set_auto_mark = 0;
          }

          if (c == '2') {
            Serial.println("OFF");
            irsend.sendRaw(OffMode, sizeof(OffMode) / sizeof(OffMode[0]), freq);
            set_auto_mark = 0;
          }

          if (c == '3') {
            Serial.println("AUTO");
            set_auto_mark = 1;
          }

          if (c == '4') {
            Serial.println("VITALY");
            irsend.sendRaw(VitalyMode, sizeof(VitalyMode) / sizeof(VitalyMode[0]), freq);
            set_auto_mark = 0;
          }

          if (c == '5') {
            Serial.println("COMFORT");
            irsend.sendRaw(ComfortMode, sizeof(ComfortMode) / sizeof(ComfortMode[0]), freq);
            set_auto_mark = 0;
          }

        }

        if (c == '\n') {                              //если "с" равен символу новой строки
          currentLineIsBlank = true;                  //то начинаем новую строку
        }
        else if (c != '\r') {                         //иначе, если "с" не равен символу возврата курсора на начало строки
          currentLineIsBlank = false;                 //то получаем символ на текущей строке
        }

        if (c == '\n' && currentLineIsBlank) {        //выводим HTML страницу
          client.println("HTTP/1.1 200 OK");          //заголовочная информация
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          //client.println("Refresh: 30");              //автоматическое обновление каждые 30 сек
          client.println();
          client.print("<!DOCTYPE html>");
          client.print("<html>");
          client.print("<head>");
          client.print("<link href=\"https://fonts.googleapis.com/css?family=Jura\" rel=\"stylesheet\">");
          client.print("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">");
          client.print("<meta http-equiv=\"refresh\" content=\"5; url=http://10.0.0.240/\">");
          client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
          client.print("<style type=\"text/css\">");
          client.print("* { margin: 0; padding: 0;  font-family: 'Jura', sans-serif;}");
          client.print("h1 {margin-left: 15px; font-size: 26px;}");
          client.print("div {border-radius: 3px; background: #51A8FF; color: white; text-align: center; padding-top: 5px; padding-bottom: 5px; width: 180px; margin-left: 15px; font-size: 22px;}");
          client.print(".button {border-radius: 3px; border: none; color: white; text-align: center;  margin-bottom: 5px; width: 88px; height: 32px; margin-left: 15px; text-decoration: none; display: inline-block; font-size: 16px; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer;}");
          client.print(".button:hover {background-color: white; color: black;}");
          client.print(".button1 {background-color: #4CAF50; border: 2px solid #4CAF50;}");
          client.print(".button2 {background-color: #f44336; border: 2px solid #f44336; margin-left: 1px;}");
          client.print(".button3 {background-color: #808080; border: 2px solid #808080; width: 177px;}");
          client.print(".button4 {background-color: #004CFF; border: 2px solid #004CFF;}");
          client.print(".button5 {background-color: #FF9000; border: 2px solid #FF9000; margin-left: 1px;}");
          client.print("@media only screen and (max-width: 425px) {");
          client.print("h1 {text-align: center;}");
          client.print("div {width: 92%; padding-top: 25px; padding-bottom: 25px;}");
          client.print(".button {width: 45%; height: 45px;}");
          client.print(".button3 {width: 91%; height: 45px;}");
          client.print("}");
          client.print("</style>");
          client.print("</head>");
          client.print("<body>");
          client.print("<title>Air-Conditioning Dashboard</title>");
          client.print("<h1>Кабинет №932</h1>");
          client.print("<br />");
          client.print("<div>");
          client.print("<p>Температура");
          client.print("<br />");
          client.print(DHT.temperature, 1);
          client.print(" C<sup>0</sup>");
          client.print("</p>");
          client.print("</div>");
          client.print("<br />");
          client.print("<div>");
          client.print("Влажность");
          client.print("<br />");
          client.print(DHT.humidity, 1);
          client.print(" %");
          client.print("<br />");
          client.print("</p>");
          client.print("</div>");
          client.print("<br /><br />");
          client.print("<a href=\"/$1\"><button class='button button1'>ВКЛ</button></a>");
          client.print("<a href=\"/$2\"><button class='button button2'>ВЫКЛ</button></a>");
          client.print("<br />");
          client.print("<a href=\"/$3\"><button class='button button3'>АВТО</button></a>");
          client.print("<br />");
          client.print("<a href=\"/$4\"><button class='button button4'>ХОЛОД</button></a>");
          client.print("<a href=\"/$5\"><button class='button button5'>КОМФОРТ</button></a>");
          client.print("</body>");
          client.print("</html>");
          break;                                      //выход
        }

      }

    }
    delay(1);                                          //время на получение новых данных
    client.stop();                                     //закрываем соеднение
  }

}
