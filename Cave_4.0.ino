#include <stdarg.h>
#include <math.h>


#include <IRremote.h>
#include <IRremoteInt.h>
 #include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#define  NINE   0xFF52AD
const int IRsensor = 13;
IRrecv irrecv(IRsensor);
decode_results results;
bool specialEffectTriggered = false;


class meter
{
  private:
    int numOfTiers;
    int tierPins[11];//11 is max availible pins, minus one for a sensor.
    int currentLevel = 0;//IMPORTANT: CurrentLevel is the highest tier lights that are on.
    void allOn()
    {
      for (int iii = 0; iii < numOfTiers; iii++) //Used iii just in case.
      {
        digitalWrite(tierPins[iii], HIGH);
      }
    }
    void allOff()
    {
      for (int iii = 0; iii < numOfTiers; iii++) //Used iii just in case.
      {
        digitalWrite(tierPins[iii], LOW);
      }
    }
    void oddOn()
    {
      allOff();
      for (int iii = 1; iii < numOfTiers; iii = iii + 2) //Used iii just in case.
      {
        digitalWrite(tierPins[iii], HIGH);
      }
    }
    void evenOn()
    {
      allOff();
      for (int iii = 0; iii < numOfTiers; iii = iii + 2) //Used iii just in case.
      {
        digitalWrite(tierPins[iii], HIGH);
      }
    }
  public:
    meter(int tiers, ...);
    void setLevel(int level);
    void flashyBlinky() const;
    void beepBeep() const;
    int getCurrentLevel() const;
    int getNumOfTiers() const;
} weightLightMeter(9, 2, 3, 4, 5, 6, 7, 8, 9, 10);//First variable is number of levels, after that, it's pins in order from bottom to top.




meter::meter(int tiers, ...)
{
  int tierNum = 0;//Measures tier at time of declaration
  va_list args;
  va_start(args, tiers);
  numOfTiers = tiers;
  while (tierNum <= numOfTiers)//Until the list ends
  {
    int pin = va_arg(args, int);
    tierPins[tierNum] = pin;
    pinMode(tierPins[tierNum], OUTPUT);
    digitalWrite(tierPins[tierNum], LOW);
    tierNum++;
  }
  va_end(args);
  for (int ii = tierNum; ii < 11; ii++)
  {
    tierPins[ii] = 0;
  }
}

void meter::setLevel(int levelToSet)
{
  Serial.print("Setting level to ");
  Serial.println(levelToSet);
  if (levelToSet >= numOfTiers || levelToSet < 0)
  {
    Serial.print("Invalid number; levelToSet is ");
    Serial.print(levelToSet);
    Serial.print(" and numOfTiers is ");
    Serial.println(numOfTiers);
  }
  while (currentLevel > levelToSet && levelToSet < numOfTiers)
  {
    if (tierPins[currentLevel] == 0)
    {
      Serial.println("AN ERROR OCCURRED");//Check that there is a value associated with tierPins[ii - 1]
#if 1
      Serial.print("You are in loop 1 of 2. currentLevel is ");
      Serial.print(currentLevel);
      Serial.println(" and the numbers in tierPins are");
      for (int ii = 0; ii < numOfTiers; ii++)
      {
        Serial.println(tierPins[numOfTiers]);
      }
#endif
      break;
    }
    digitalWrite(tierPins[currentLevel - 1], LOW);
    currentLevel--;
    Serial.print("Level lowered to ");
    Serial.println(currentLevel);
    delay(100);
  }
  while (currentLevel < levelToSet && levelToSet >= 0)
  {
    //Note: I don't have tierPins[currentLevel - 1] because that wouldn't work out.
    if (( tierPins[currentLevel]) == 0 )
    {
      Serial.println("AN ERROR OCCURRED");//Check that there is a value associated with tierPins[ii - 1]
#if 1
      Serial.print("You are in loop 2 of 2. currentLevel is ");
      Serial.println(tierPins[currentLevel]);
#endif
    }
    digitalWrite(tierPins[currentLevel], HIGH);
    currentLevel++;
    Serial.print("Level raised to ");
    Serial.println(currentLevel);
    delay(100);
  }
}
void meter::flashyBlinky() const
{
  for (int jj = 0; jj < 2; jj++)
  {
    allOff();
    for (int kk = 0; kk < 2; kk++)
    {
      for (int ii = 0; ii < numOfTiers; ii++)
      {
        digitalWrite(tierPins[ii], HIGH);
        delay(75);
        digitalWrite(tierPins[ii], LOW);
      }
      for (int ii = numOfTiers - 2; ii > 0; ii--)
      {
        digitalWrite(tierPins[ii], HIGH);
        delay(75);
        digitalWrite(tierPins[ii], LOW);
      }
    }
    oddOn();
    delay(250);
    evenOn();
    delay(250);
    oddOn();
    delay(250);
    evenOn();
  }
  allOff();
  for (int ii = 0; ii < getCurrentLevel(); ii++)
  {
    digitalWrite(tierPins[ii], HIGH);
  }
}
void meter::beepBeep() const
{
  allOn();
  delay(500);
  allOff();
  for (int ii = 0; ii < getCurrentLevel(); ii++)
  {
    digitalWrite(tierPins[ii], HIGH);
  }
}
int meter::getCurrentLevel() const
{
  return currentLevel;
}

int meter::getNumOfTiers() const
{
  return numOfTiers;
}

void specialEffect()
{
  if (specialEffectTriggered == false)
  {
    digitalWrite(13, HIGH);//Just pretend 12 is the special effect trigger
    specialEffectTriggered = true;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(IRsensor, INPUT);
  irrecv.enableIRIn();
}

void loop()
{
  if  (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    if (results.value == NINE)
    {
      weightLightMeter.flashyBlinky();
      }
    }
    irrecv.resume();
  }
