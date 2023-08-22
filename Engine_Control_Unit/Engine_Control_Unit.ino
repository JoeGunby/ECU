int ThrottlePin = A0;
const int InletHallPin = 2;
const int ExhaustHallPin = 3;
int Relay2 = 10;
int Relay3 = 11;

int ThrottlePosition = 0;
int long InletValveOpeningTime = 0;
int long ExhaustValveOpeningTime = 0;
int long CurrentTDC, TimeSinceCurrentTDC;
int long PreviousTDC = 0;
bool InletTrip = false;
bool ExhaustTrip = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(InletHallPin, INPUT);
  pinMode(ExhaustHallPin, INPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  digitalWrite(Relay2, LOW);
  digitalWrite(Relay3, LOW);
  attachInterrupt(digitalPinToInterrupt(InletHallPin), InletValveTrigger, FALLING);
  attachInterrupt(digitalPinToInterrupt(ExhaustHallPin), ExhaustValveTrigger, FALLING);
}

void loop() {
  InletValveOpening();
  ExhaustValveOpening();
}


void InletValveOpening() {
// Delay factor for valve trigger  
  if (InletTrip == true) {
    InletValveOpeningTime = CalculateValveOpeningTime();
    digitalWrite(Relay2, HIGH);
    TimeSinceCurrentTDC = millis() - CurrentTDC;
    if (TimeSinceCurrentTDC >= InletValveOpeningTime) {
      digitalWrite(Relay2, LOW);
      Serial.print("Time Running: ");
      Serial.print (CurrentTDC);
      Serial.print("\t Rev Split Time: ");
      Serial.print(TimeSinceCurrentTDC);
      Serial.print("\t Valve Opening Time: ");
      Serial.print(InletValveOpeningTime);
      Serial.print("\t Throttle Position: ");
      Serial.println(ThrottlePosition);
      InletTrip = false;
    }
  }
}

void ExhaustValveOpening() {
  // Delay factor for valve trigger  
  if (ExhaustTrip == true) {
    ExhaustValveOpeningTime = CalculateValveOpeningTime();
    digitalWrite(Relay3, HIGH);
    TimeSinceCurrentTDC = millis() - CurrentTDC;
    if (TimeSinceCurrentTDC >= ExhaustValveOpeningTime) {
      ExhaustTrip = false;
    }

}

void InletValveTrigger() {
  //millis won't run within an interupt therefore must be in loop.
  //setting values for the trigger in the loop to be called.
  InletTrip = true;
  PreviousTDC = CurrentTDC;
  CurrentTDC = millis();
}

void ExhaustValveTrigger() {
  //millis won't run within an interupt therefore must be in loop.
  //setting values for the trigger in the loop to be called.
  ExhaustTrip = true;
  PreviousTDC = CurrentTDC;
  CurrentTDC = millis();
}

int CalculateValveOpeningTime() {
  //Set default value of .2 for now as a proportion of the stroke to open the valve,
  //needs a feedback loop to vary the value based on time since last TDC.
  int StartUpValveOpening = 200;
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





