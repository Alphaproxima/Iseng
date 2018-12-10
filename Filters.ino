// Filtering the distance sensor
// using Kalman Filter algorithm
// Decision make by using information entropy
// The final decision using binarization

#include<math.h>

  int i;  //iterations
  float y[] = {0,0,0,0,0};  // output
  
  //define the pin port
  const int trigger = 11;
  const int echo = 12;

  //define variable
  double duration, distance;
  double H; //Shannon entropy parameter
  double KL; //Kullback Liebler parameter for future use
  double gd; //gaussian distribution
  double sd; //standard deviation
  double mean;
  
  // Kalman Filter
  double R = 6e-3, Q = 1e-4;
  double Xpe0 = 0.0, Xe1 = 0.0, Xe0 = 0.0;
  double P1 = 1.0, Ppe0 = 0.0, P0 = 0;
  double  K = 0.0, Z = 0.0; 
  double kal_fil;
  
void setup() {
  // HC-SR04 pin function
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
 
  // LED For intepretation of approaching obstacle
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
    // Clears the trigger
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);

    // Sets the trigger on HIGH state for 10 micro seconds
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
          
    // Reads the echo, returns the sound wave travel time in microseconds
    duration = pulseIn(echo, HIGH);

    // Calculating the distance in cm
    distance= duration*0.0343/2.0;
    Serial.print(distance);
    Serial.print(" ");
//    Serial.print(12.5);
//    Serial.print(" ");
    
    // Kalman Filter
    // The parameters below define the kalman iteration
    // This method works under an iteration of prediction and
    // estimated error sensor reading
    Z = duration;
    Xpe0 = Xe1;
    Ppe0 = P1 + Q;
    K = Ppe0/(Ppe0+R);
    Xe0 = Xpe0 + K * (Z-Xpe0);
    P0 = (1-K)*Ppe0;
    kal_fil = Xe0*0.0343/2.0;
    Serial.print(kal_fil);
    Serial.print(" ");
    Xe1 = Xe0; P1 = P0;

    // Decision making using decrement of information entropy
    // H(x) = -Sum(i to N) p1 log2(p1)
    for (i=0; i<=4; i++){
      y[i+1] = y[i];
    } 
    y[0] = kal_fil; // raw data after estimated

    // Calculate the entropy
    H =-((y[0]*(log(y[0])/log(2))+y[1]*(log(y[1])/log(2))
      +y[2]*(log(y[2])/log(2))+y[3]*(log(y[3])/log(2))
      +y[4]*(log(y[4])/log(2))))/5;      
//    Serial.print(H);
//    Serial.print(" ");
    
    //Normalized the entropy
    //Hp[x] = H(x)/ln N 
    double norm = H/(log(5)/log(2));
    double KL = H*log((H/kal_fil));
    Serial.println(norm);
    
    // Kullback-Leibler Divergence
    // Data comparison for necessary compression

    if(norm <= -500){
      // Define that a distance between quadrotor and object
      // is still far. So the quadrotor will stay on the position
      digitalWrite(LED_BUILTIN, LOW);
    }

    else{
      // Define that a distance between quadrotor and object
      // is near. So the quadrotor should avoid or take an action
      digitalWrite(LED_BUILTIN, HIGH);
    }
    
    delay(50);
}
