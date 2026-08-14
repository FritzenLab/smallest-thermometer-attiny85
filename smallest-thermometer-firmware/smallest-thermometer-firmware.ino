#define pinZero 0
#define pinUno 1

// Conexão do termistor
const int pinTermistor = A1;
 
// Parâmetros do termistor
// page three of the B57421V2103 datasheet https://product.tdk.com/system/files/dam/doc/product/sensor/ntc/chip-ntc-thermistor/data_sheet/50/db/ntc/ntc_smd_standard_series_0805.pdf
const double beta = 3940.0;
const double r0 = 10000.0;
const double t0 = 273.0 + 25.0;
const double rx = r0 * exp(-beta/t0);
 
// Parâmetros do circuito
double vcc = 5.0;
const double R = 10000.0;
 
// Numero de amostras na leitura
const int nAmostras = 5;

int bitsresultados[6];
int passagem= 0;

// Variables used on this code
unsigned long time1;
unsigned long previousTime;
boolean enterFunction = true;
unsigned long time2;
unsigned long previousTime2;
boolean enterFunction2 = true;
//-----------------------

bool blinkingtens= false;
bool enteredtens= false;
int doubletens= 0;
bool blinkingunits = false;
bool enteredunit = false;
int doubleunit = 0;
int unit = 0;
int tens = 0;
bool waittime = true;
bool startedwait = false;
long elapsedtime;
int temperature;
float smoothntc = 0;
int digitoaentrar = 1;

class MovingAverage {
  private:
    int _numReadings;
    float *_readings;     
    int _readIndex = 0;
    float _total = 0; 
    int _startupCounter = 0;

  public:
    MovingAverage(int size) {
      _numReadings = size;
      _readings = new float[_numReadings];
      for (int i = 0; i < _numReadings; i++) _readings[i] = 0.0;
    }

    ~MovingAverage() {  // free memory
      delete[] _readings;
    }

    float update(float newValue) {
	    _startupCounter ++;
      _total -= _readings[_readIndex];
      _readings[_readIndex] = newValue;
      _total += newValue;

      _readIndex++;
      if (_readIndex >= _numReadings) _readIndex = 0;

      if(_startupCounter < _numReadings){
        return _total / _startupCounter;
      }else{
        return _total / _numReadings; 
      }
    }
};

MovingAverage ntcAvg(15);

void setup() {
  
  //Serial.begin(9600);
  pinMode(pinZero, OUTPUT);
  pinMode(pinUno, OUTPUT);
}

void loop() {

  time1 = micros();
  time2 = micros();
  if (enterFunction == true) {
    previousTime = time1;
    passagem++;
    // Start your code below
    //-----------------------

    if(blinkingtens == true && blinkingunits == false){
      if(enteredtens == false){
        enteredtens = true;
        doubletens = 2 * tens;
        if(doubletens == 0){
          doubletens= 1;
        }
      } 
      doubletens --;
      if(doubletens != 0){
        digitalWrite(pinZero, !digitalRead(0));
      }else{
        blinkingunits = true;
        blinkingtens = false;
      }

    }else if(blinkingunits == true && blinkingtens == false){
      blinkingtens= false;
      if(enteredunit == false){
        enteredunit = true;
        doubleunit = 2 * unit;
        if(doubleunit == 0){
          doubleunit= 1;
        }
      }
      doubleunit --;
      if(doubleunit != 0){
        digitalWrite(pinUno, !digitalRead(1));
      }else{
        waittime = true;
        blinkingunits = false;
        startedwait = true;
      }

    }else if(waittime == true){
      if(startedwait == true){
        digitalWrite(pinUno, LOW);
        digitalWrite(pinZero, LOW);
        startedwait = false;
        elapsedtime = millis();
      }
      if(millis() - elapsedtime > 998){
        waittime = false;
      }

    }else{
      blinkingtens= true;
      enteredtens = false;
      enteredunit = false;
    }

    
    
      
    

    //-----------------------
    // End of your code
  }

  if (enterFunction2 == true && waittime == true) { //Enter this function every xx milisseconds and IF LEDs are not blinking
    previousTime2 = time2;

    // Le o sensor algumas vezes
    int soma = 0;
    for (int i = 0; i < nAmostras; i++) {
      soma += analogRead(pinTermistor);
      delay (100);
    }

    double v = (vcc*soma)/(nAmostras*1024.0);
    double rt = (vcc*R)/v - R;
 
    double t = beta / log(rt/rx);
    t= t - 273;
    temperature = int(t);
    smoothntc = ntcAvg.update(temperature);

    if(smoothntc < 10){
      tens= 0;
      unit= smoothntc;
    }else if(smoothntc >= 10 && smoothntc < 20){
       tens= 1;
       unit= smoothntc - 10;
    }else if(smoothntc >= 20 && smoothntc < 30){
      tens= 2;
      unit= smoothntc - 20;
    }else if(smoothntc >= 30 && smoothntc < 40){
      tens= 3;
      unit= smoothntc - 30;
    }else if(smoothntc >= 40 && smoothntc < 50){
      tens= 4;
      unit= smoothntc - 40;
    }else{
      tens= 0;
      unit= 0;
    }
  }


  // The DELAY time is adjusted in the constant below >>
  if (time1 - previousTime < 399990) { // 1 million microsencods= 1 second delay
    /* I have actually used 0.999990 seconds, in a trial to compensate the time that
       this IF function takes to be executed. this is really a point that
       need improvement in my code */
    enterFunction = false;
  }
  else {
    enterFunction = true;
  }
  if (time2 - previousTime2 < 99990) { // 1 million microsencods= 1 second delay
    /* I have actually used 0.999990 seconds, in a trial to compensate the time that
       this IF function takes to be executed. this is really a point that
       need improvement in my code */
    enterFunction2 = false;
  }
  else {
    enterFunction2 = true;
  }

}
