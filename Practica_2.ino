// BLUETOOTH
#include <BluetoothSerial.h>  // Librería Bluetooth
// SERVOMOTOR
#include <ESP32Servo.h>       //Ponemos la librería
#define servoPin 12           //Se define el pin del servomotor
// DHT11
#include <Adafruit_Sensor.h>  // Controlador de sensor unificado Adafruit
#include <DHT.h>              // Librería DHT para usar sensor de humedad y temperatura
#include <DHT_U.h>            // Libreria DHT
#define DHTTYPE 11            // Define el tipo de sensor DTH
#define DHTPIN 15             // Definimos el pin del DTH11
int relay = 22;               // Definimos el pin para el relay

// LED Y FOTORESISTENCIA
#define LEDPIN 22             //Definimos el pin del LED
#define FPIN 4               // Definimos el pin de la fotoresistencia

//Sensor de Distancia
const int Trig = 2;  		//Pin digita para el Trigger del sensor
const int PinEcho = 21;    	//Pin digital para el Echo del sensor

//Comandos para verificar si se encuentra ahabilitado el bluetooh
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
#endif

// Instancia de Bluetooth Serial
	BluetoothSerial SerialBT;
// Variable para encender el led          
	String letra;     
// Variables float para almacenar el valor de la temperatura y humedad                 
	float temperatura, humedad;        
// Variable int para el valor de la luminosidad
	int luminosidad = 0;               
// Variable para el tiempo 
	unsigned long tiempo1 = 0;     
// Variable para el tiempo     
	unsigned long tiempo2 = 0;     
// Variable para el tiempo     
	unsigned long tiempoSegundos = 0;  
// Variable int para la frecciencia/velocidad del giro del servomotor
	int frecuencia = 5;   
// Instancia de Servo             
	Servo servoM;   
// Variable int que almacena el angulo del servomotor                   
	int data;  
// Variable para el menú                        
	int contador = 0;                  
	long t; //tiempo que demora en llegar el eco
	long d; //distancia en centimetros
// Se inicializa el sensor DHT (Pin, Tipo)
	DHT dht(DHTPIN,DHTTYPE);           

//Primera función a ejecutar en el programa y se utiliza para configuurar, inicializar variables, comenzar a usar librerías, etc.
void setup() {
  	contador = 0;
  // Se inicializa el sensor
  	dht.begin();    
  // Puerto Serie para bluetooh                   
  	Serial.begin(115200);    
  // Nombre del despisitivo Bluetooth          
  	SerialBT.begin("esp32Omar");   
  // Establece el pin 5 como salida IN1    
  	pinMode(LEDPIN,OUTPUT); 
  // Obtiene el tiempo transcurrido desde que se prendió la ESP32           
  	tiempo1 = millis();             
  // Se le indica al servomotor el pin   
  	servoM.attach(servoPin);  
  // Se pone el servomotor en 0º           
  	servoM.write(0);                  
  //Sensor distancia
  	pinMode(Trig, OUTPUT); //pin como salida
  	pinMode(PinEcho, INPUT);  //pin como entrada
  	digitalWrite(Trig, LOW);//Inicializamos el pin con 0
  	pinMode(relay, OUTPUT);
}
      
//Contiene el código que se ejecutará continuamente (lectura de entradas, activación de salidas, etc)
	void loop() {
  // Se obtiene los segundos transcurridos
  	tiempo2 = millis();       
  // Se obtiene la humedad                     
  	humedad = dht.readHumidity();  
  // Se obtiene la temperatura                
  	temperatura = dht.readTemperature();  
  // Se obtiene la luminosidad         
  	luminosidad = analogRead(FPIN);                
  
  // Si ha pasado 1 segundo ejecuta el IF
  	if(tiempo2 > (tiempo1 + 10000)){      
    	tiempo1 = millis();                 // Actualiza el tiempo actual
    	tiempoSegundos = tiempo1/1000;      // Calcula el tiempo en segundos desde que se prendio el ESP32, es decir, millis()/1000
  
    	if (isnan(humedad) || isnan(temperatura) || isnan(luminosidad) || isnan(d)){      //Se comprueba que los valores de temperatura son igual a nan
      SerialBT.println(F("Error al obtener la temperatura y humedad")); //En caso de ser así mostrará un mensaje de error
      }else{  //En caso de ser diferente de nan, se muestran cada uno de los valores
   //Sensor DHT11
   // Se imprime un mensaje  
	SerialBT.print("Humedad: ");      
   // Se imprime el valor de la humedad  
	SerialBT.println(humedad);        
   // Se imprime un mensaje  
	SerialBT.print("Temperatura: ");
   // Se imprime el valor de la temperatura  
      SerialBT.print(temperatura);      
   // Se imprime un mensaje
	SerialBT.println(" ºC");      
   // Se imprime un mensaje    
        SerialBT.print("Luminosidad: ");  
   // Se imprime el valor de la luminosidad
        SerialBT.println(luminosidad);    
      }

      if(isnan(d)){                                           //Se comprueba que los valores de temperatura son igual a nan
        SerialBT.println("Error al obtener la distancia");	 //En caso de ser así mostrará un mensaje de error
      }else{  								//En caso de ser diferente de nan, se muestran cada uno de los valores
        //Sensor de distancia
        digitalWrite(Trig, HIGH);
        delayMicroseconds(10);          				//Enviamos un pulso de 10us
        digitalWrite(Trig, LOW);
        t = pulseIn(PinEcho, HIGH); 				//obtenemos el ancho del pulso
        d = t/59;             					//escalamos el tiempo a una distancia en cm
        SerialBT.print("Distancia: ");
        SerialBT.print(d);      					//Enviamos serialmente el valor de la distancia
        SerialBT.print(" cm");
        SerialBT.println();
        delay(100);         						//Hacemos una pausa de 100ms
      }
  }
    
  if (SerialBT.available()){                 // Si se recibe algo de bluetooth 
    letra = SerialBT.readStringUntil('\n');  // Se almacena en la variable letra
    
    if(letra=="On" || letra=="on" || letra=="ON"){    // Si el valor es igual a 1
      digitalWrite(LEDPIN,LOW);                       // Se enciende el led
      digitalWrite(relay, LOW);
      delay(100); 
      Serial.println("Apagado");
    }                            
    if(letra=="Off" || letra=="off" || letra=="OFF"){ // Si el valor es igual a 0
      digitalWrite(LEDPIN,HIGH);                      // Se apaga el led
      digitalWrite(relay, HIGH);
      Serial.println("Encendido");
      delay(100);
    }
    if (letra.toInt() > 0 and letra.toInt() <= 180){ // Si la letra puede convertir en número y el número es menor a 180º
        data = letra.toInt(); // Se guarda en el valor en data
        ServoMotor();         // Se llama al metodo ServoMotor
    }
    if (letra == "0"){        // Si el valor es igual a 0
        data = 0;             // data será igual a 0
        ServoMotor();         // Se llama al metodo ServoMotor
    }
    if (letra.toInt() > 0 and letra.toInt() > 180){ // Si la letra puede convertir en número y el número es menor a 180º
        data = 180; // Se guarda en el valor en data
        ServoMotor();         // Se llama al metodo ServoMotor
    }
    
   if(letra != "" || letra != NULL){ // Si la variable es diferente nulo
    Serial.println(letra);           // Se muestra en el Monitor Serie
    }     
  }
}

// Método para mover el servomotor
void ServoMotor(){
  if(data < 180){        // Si data es menor a 180º
    servoM.write(data);  // Se mueve el servomotor a ese angulo
  }else if(data >= 180){ // Si  data es mayor o igual a 180º
    servoM.write(180);   // Se mueve el servomotor a 180º
  }
}
