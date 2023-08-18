int ThrottlePin = A0;
const int HallPin = 2;
int Relay2 = 10;

int ThrottlePosition = 0;
int long ValveOpeningTime = 0;
int long CurrentTDC, TimeSinceCurrentTDC;
int long PreviousTDC = 0;
bool Trip = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(HallPin, INPUT);
  pinMode(Relay2, OUTPUT);
  digitalWrite(Relay2, LOW);
  attachInterrupt(digitalPinToInterrupt(HallPin), ValveTrigger, FALLING);
}

void loop() {
  ValveOpening();
}


void ValveOpening() {
// Delay factor for valve trigger  
  if (Trip == true) {
    ValveOpeningTime = CalculateValveOpeningTime();
    digitalWrite(Relay2, HIGH);
    TimeSinceCurrentTDC = millis() - CurrentTDC;
    if (TimeSinceCurrentTDC >= ValveOpeningTime) {
      digitalWrite(Relay2, LOW);
      Serial.print("Time Running: ");
      Serial.print (CurrentTDC);
      Serial.print("\t Rev Split Time: ");
      Serial.print(TimeSinceCurrentTDC);
      Serial.print("\t Valve Opening Time: ");
      Serial.print(ValveOpeningTime);
      Serial.print("\t Throttle Position: ");
      Serial.println(ThrottlePosition);
      Trip = false;
    }
  }
}


void ValveTrigger() {
  //millis won't run within an interupt therefore must be in loop.
  //setting values for the trigger in the loop to be called.
  Trip = true;
  PreviousTDC = CurrentTDC;
  CurrentTDC = millis();
  }

int CalculateValveOpeningTime() {
  //Set default value of .2 for now as a proportion of the stroke to open the valve,
  //needs a feedback loop to vary the value based on time since last TDC.
  int StartUpValveOpening = 500;
  float ValveOpeningMultiplier = CalculateValveOpeningMultiplier();
  //float ThrottleMultiplier = CalculateThrottleMultiplier();
  if (PreviousTDC == 0) {
    return (StartUpValveOpening);
  }
  if (CurrentTDC - PreviousTDC >= 2000) {
    return (StartUpValveOpening);
  }
  return ((CurrentTDC - PreviousTDC) * ValveOpeningMultiplier/* * ThrottleMultiplier*/);
}

float CalculateValveOpeningMultiplier() {
  if (CurrentTDC - PreviousTDC < 50) {
    return(.05);
  }
  else if (50 <= CurrentTDC - PreviousTDC < 100) {
    return(.1);
  }
  else {
    return(.2);
  }
}

// float CalculateThrottleMultiplier() {
//   float IdleThrottleMultiplier = 1;
//   ThrottlePosition = analogRead(ThrottlePin);
//   if (ThrottlePosition <= 50) {
//     return(IdleThrottleMultiplier);
//   }
//   else {
//     return (ThrottlePosition / 5000);
//   } 
// }





