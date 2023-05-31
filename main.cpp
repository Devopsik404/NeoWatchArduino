//Подключаем libы
#include "Adafruit_LEDBackpack.h"
#include <Adafruit_NeoPixel.h>
//Установка 
#define PIN_MINUTES 3
#define NUM_MINUTES 12
#define PIN_HOURS 6
#define NUM_HOURS 24
#define NUM_LEDS 7
#define PIN_LEDS 5
#define BUTTON_PIN 8
#define LIGHT_SENSOR A1
#define TMP_SENSOR A0

//Подключаем екраны и NeoPixel
Adafruit_7segment clock = Adafruit_7segment();
Adafruit_NeoPixel pixels(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(NUM_MINUTES, PIN_MINUTES, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel hours_panel(NUM_HOURS, PIN_HOURS, NEO_GRB + NEO_KHZ800);

//КАЛИБРОВКА 
int hours = 8;
int minuts = 30;
int seconds = 0;
//КАЛИБРОВКА

int mod = 0;

float voltage, temperature;

void setup() {
  //Усатнвока ПИНов
  	pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(10, INPUT);
 //Запуск NeoPixel
    hours_panel.begin();
  	strip.begin();
    Serial.begin(9600);
  //Зажигаем часы
  	setHours();
  	setMinutes();
	//Запуск моника
    clock.begin(112);
}

int getColor(){
	//устанавливаем режим
    if (mod == 0) {
    return pixels.Color(random(256), random(256), random(256));
  } else if (mod == 1) {
    return pixels.Color(120, 120, 2);
  } else if (mod == 2) {
    return pixels.Color(0, 220, 0);
  } else if (mod == 3) {
    return pixels.Color(0, 0, 255);
    } else if (mod == 4){
    }
}

void readButton(){
	if (digitalRead(BUTTON_PIN) == HIGH) { // если кнопка нажата
    mod++; // увеличиваем счетчик на 1
    Serial.println(mod);
    if (mod == 5) { // если значение счетчика больше 3
      mod = 0; // обнуляем его
    }
	//Показываем время
    setHours();
    setMinutes();
    setSeconds();
    }
}

void loop() {
	//запсукаем время
  	newTime(10);
	//показываем секунды
    setSeconds();
	//получаем цветовой режим
  	getColor();
	//Полуаем показания с термометра
  	setTemperature();
    delay(500); //для рельаных часов надо 1000
  	//считываем нажатия кнопки, переключения режима
  	readButton();
  	//Авто-яркость
  	lightUpNeoPixel(analogRead(LIGHT_SENSOR));
    //Serial.println(analogRead(LIGHT_SENSOR));
}

void setHours() {
  // Зажигаем  количество светодиодов в цвете RGB
    for (int i = 0; i < hours; i++) {
        if(mod <= 3){
    hours_panel.setPixelColor(i,getColor());
    } else {
    	hours_panel.setPixelColor(i,hours_panel.Color(0,0,255));
    }
    }
    if(hours == 1){
    for(int j = 0; j < NUM_HOURS;j++){
    	hours_panel.setPixelColor(j, hours_panel.Color(0, 0, 0));
    }
    hours_panel.setPixelColor(1, hours_panel.Color(255, 0, 255));
  }
    hours_panel.show();
}

void setMinutes() {
  // Зажигаем  количество светодиодов в случайном цвете RGB
  int num_leds = (minuts / 60.0) * 12;
  for (int i = 0; i < num_leds; i++) {
    if(mod <= 3){
    strip.setPixelColor(i,getColor());
    } else {
    	strip.setPixelColor(i,strip.Color(0,255,0));
    }
  }
  strip.show();
}

//Автояркость
void lightUpNeoPixel(int number) {
  //если темно то якро светить не надо
  if (number == 0){
  	pixels.setBrightness(25);
  	pixels.show();
  	strip.setBrightness(25);
 	strip.show();
	hours_panel.setBrightness(25);
 	hours_panel.show();
  }
  else{
	//выводим новуюю яроксть в зависимости от показаний датчика
  int brightness = map(number, 64, 85, 25, 255);
  pixels.setBrightness(brightness);
  pixels.show();
  strip.setBrightness(brightness);
  strip.show();
  hours_panel.setBrightness(brightness);
  hours_panel.show();
  }
}

void newTime(int counter) {
  //увеличиваем секунды
   seconds+=counter;
  //увеличиваем минуты
  if(seconds >= 60){
    seconds = 0 ;
    //обнуляем круг 
    for (int i = 1; i < 7; i++){
  			pixels.setPixelColor(i, pixels.Color(0,0,0));
            pixels.show();
 	}
    //увеличиваем минуты
    minuts++;
    //показываем минуты
    setMinutes();
    if (minuts >= 60) {
        minuts = 0;
		//обнуляем секндый круг
      	for (int i = 0; i < 12; i++){
  			strip.setPixelColor(i, strip.Color(0,0,0));
           strip.show();
 		}
		//увеличиваем часы
        hours++;
      setHours();
      //обнуляем сутки
        if (hours >= 24) {
            hours = 0;
          for (int j = 0; j < NUM_HOURS; j++){
  				    hours_panel.setPixelColor(j, hours_panel.Color(0, 0, 0));
            	Serial.println(j);
            	hours_panel.show();
 			}
        }
    }
  }
 
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minuts);
  Serial.println();
  //показываем на моник
  clock.print(hours*100+minuts);
  clock.writeDisplay();
}
//показываем термометр
void setTemperature(){
	//получаем данные с термометра
  voltage = analogRead(TMP_SENSOR) * 0.0048828125;
  temperature = (voltage - 0.5) * 100;
  //зажигаем лампочку в зависимости от показаний
  if (temperature > 35) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Red
  } else if (temperature > 20) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 165)); // 
  } else if (temperature > 15) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 255)); // 
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Blue
  }
  pixels.show();
}
//показываем секунды
void setSeconds(){
  //замечательная математика
  int num_leds = (seconds / 60.0) * 6+1;
	//зажигаем лампочки
  for (int i = 1; i < num_leds+1; i++) {
    if(mod <= 3){
    pixels.setPixelColor(i,getColor());
    } else {
    	pixels.setPixelColor(i,pixels.Color(255,0,0));
    }
  }
  //показываем лампочкт
  pixels.show();
}
