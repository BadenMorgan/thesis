/*
    prototype program
    by Baden Morgan
    2016
    UCT
    This program is to help observe the characteristics of the
    delivery mechanism for the UCT vending machine. It uses the
    Arduino platform in order to speed up the prototyping
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 45;    // variable to store the servo position
int temppos = 45;
int drop = 45;
int pick = 45;
byte routine = 0;
char character;
String content = "";
long Stamp = 0;
void setup() {
  Serial.begin(9600);
  Serial.println("hello");
  myservo.write(pos);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(7, OUTPUT);
}

void loop() {
  while (Serial.available() > 0) {

    switch (routine) {
      case 0:
        {
          while (Serial.available()) {
            character = Serial.read();
            content.concat(character);
            delay(10);
          }

          if (content != "") {
            if (content == "set") {
              routine = 1;
              content = ""; //Empty string
              break;
            }
            if (content == "go") {
              routine = 2;
              content = ""; //Empty string
              break;
            }
            if (content.toInt() > 0) { //Make sure input is a number greater than zero
              byte value = content.toInt();
              Serial.print("value: ");
              Serial.println(value);
              pos = value;
            } else {
              Serial.println("Input must be a number greater than 0");
            }
            content = ""; //Empty string
          }
          break;
        }
      case 1:
        {
          break;
        }
      case 2:
        {
          while (Serial.available()) {
            character = Serial.read();
            content.concat(character);
            delay(10);
          }

          if (content != "") {
            if (content == "g") {
              digitalWrite(7, HIGH);
              delay(500);              
              pos = drop;
            }
            if (content == "set") {
              routine = 1;
            }
            if ( content == "on") {
              digitalWrite(7, HIGH);
            }
            if ( content == "off") {
              digitalWrite(7, LOW);
            }
            content = ""; //Empty string
          }
          break;
        }
      default:
        break;
    }
  }

  if (routine == 1) {
    Serial.println("enter a IC release value:");
    //Read Input Buffer
    while (!Serial.available());
    while (Serial.available()) {
      character = Serial.read();
      content.concat(character);
      delay(10);
    }

    if (content != "") {
      if (content.toInt() > 0) { //Make sure input is a number greater than zero
        byte value = content.toInt();
        Serial.print("value: ");
        Serial.println(value);
        drop = value;
      } else {
        Serial.println("Input must be a number greater than 0");
      }
      content = ""; //Empty string
    }

    Serial.println("enter a IC load value:");
    while (!Serial.available());
    while (Serial.available()) {
      character = Serial.read();
      content.concat(character);
      delay(10);
    }

    if (content != "") {
      if (content.toInt() > 0) { //Make sure input is a number greater than zero
        byte value = content.toInt();
        Serial.print("value: ");
        Serial.println(value);
        pick = value;
      } else {
        Serial.println("Input must be a number greater than 0");
        Serial.println(content);
      }
      content = ""; //Empty string
    }
    routine = 2;
  }

  if ((millis() - Stamp ) >= 2) {
    if (pos != -1) {
      if (temppos > pos) {
        temppos--;
      } else {
        temppos++;
      }
      /*if (temppos - pos == 5 || temppos - pos == -5) {
        myservo.write(temppos + 20);              // tell servo to go to position in variable 'pos'
        delay(100);
        myservo.write(temppos = pos);              // tell servo to go to position in variable 'pos'
        } else {*/
      myservo.write(temppos);              // tell servo to go to position in variable 'pos'
      //}
      if (temppos == pos && pos == pick && routine == 2) {
        pos = -1;
        delay(500);
        digitalWrite(7, LOW);
      } else if (temppos == pos && routine == 2) {
        delay(500);
        pos = pick;
      } else if (temppos == pos) {
        pos = -1;
      }
    }
    Stamp = millis();
  }

}

