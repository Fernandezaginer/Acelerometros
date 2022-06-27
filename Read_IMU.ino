// Filtro complementario MPU6050

#include <Wire.h>

const int MPU6050 = 0x68; // Direccion I2C
float X_out, Y_out, Z_out;
float RX_out, RY_out, RZ_out;

// Offset a calibrar para el acelerometro
#define OFFSET_X 0.00
#define OFFSET_Y 0.00
#define OFFSET_Z 0.00

#define OFFSET_GX -289.83
#define OFFSET_GY 0.00
#define OFFSET_GZ 0.00



void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  
  if(!MPU6050_16g_init()){
    Serial.print("Error: MPU6050 no encontrado");
    while(true);
  }

  /*
  Serial.println("Calibrando...");
  const int n = 30000;
  float val_cal = 0.0;
  for(int i = 0; i < n; i++){
    MPU6050_16g_read_raw();
    val_cal += RX_out;
  }
  val_cal = val_cal/((float)n);
  Serial.println(val_cal);
  while(1);
*/
  
}


void loop() {
  MPU6050_16g_read_raw();
  
  Serial.print(DLPF(read_angle_x()));
  Serial.write('\n');
  delay(25);
}



float read_angle_x() {
  static unsigned long m_o = 0;
  static float Gx = 90.0;
  Gx = Gx - ((((float)RX_out) - OFFSET_GX) * 0.007907 * (millis() - m_o) * 0.0075 / 8.0);
  Gx = 0.96*Gx + 0.04*read_acc_x();
  m_o = millis();
  return Gx;
}

float read_acc_x(){
  return (atan2(Z_out,Y_out)*(180.0/3.1416));
}


float DLPF(float input){
  static float m_o = 90.0;
  float out = (m_o*0.8) + (input*0.2);
  m_o = input;
  return out;
}


void MPU6050_16g_read_raw() {

  Wire.beginTransmission(MPU6050);
  Wire.write(0x3B); // Registro I2C
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 6, true);
  X_out = (float)(Wire.read() << 8 | Wire.read());
  X_out = (X_out / 2048) - OFFSET_X;
  Y_out = (float)(Wire.read() << 8 | Wire.read());
  Y_out = (Y_out / 2048) - OFFSET_Y;
  Z_out = (float)(Wire.read() << 8 | Wire.read());
  Z_out = (Z_out / 2048) - OFFSET_Z;

  Wire.beginTransmission(MPU6050);
  Wire.write(0x43); // Registro I2C
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 6, true);
  RX_out = (float)( Wire.read() << 8 | Wire.read());
  RY_out = (float)( Wire.read() << 8 | Wire.read());
  RZ_out = (float)( Wire.read() << 8 | Wire.read());
  
}

boolean MPU6050_16g_init() {
  
  Wire.beginTransmission(MPU6050);
  if (Wire.endTransmission() == 2) {
    return 0;
  }

  // Rango máximo +-16g
  Wire.beginTransmission(MPU6050);
  Wire.write(0x1C);
  Wire.write(B00011000);
  Wire.endTransmission();
  
  return 1;
}
