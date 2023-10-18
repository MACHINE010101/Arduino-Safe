/*  Author: Eduard Pascale
    Date: 10/13/2021
    Title: The Safe Assignment
*/

//libraries

#include "Display.h"

//Variables Declaration

const int  RED_LED = 4;
const int  GREEN_LED = 5;
const int  YELLOW_LED = 7;
const int KEY1 = 8;
const int KEY2 = 9;
const int LDR = A2;
const int BUZZER = 3;

int last_btn_state = HIGH;
int current_btn_state;
bool btn_press = false;
int state = 0;
int digit = 0;
int safe_door;
int right_code = 1444;
int code = 0;
int error = 0;
int counter = 0;
bool right_pass = false;
bool mode_input = false;
bool alarm = false;
int nd_last_btn_state = HIGH;
int nd_current_btn_state;

int nr_1 = 0;
int nr_2 = 0;
int nr_3 = 0;
int nr_4 = 0;

void setup() {

  //Set pin modes and serial

  Serial.begin(9600);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY2, INPUT_PULLUP);
  pinMode(LDR, INPUT);
  pinMode(BUZZER, OUTPUT);
  Display.clear();
}

void loop() {

  safe_door = analogRead(LDR);
  safe_door = map(safe_door, 0, 1023, 0, 100);

  //This is the base mode where the safe it's open and unlocked

  if (safe_door > 30 && mode_input == false && alarm == false) {
    Display.show(" ");
    digitalWrite(GREEN_LED, LOW);
    mode_input = true;
    alarm = false;
    right_pass = false;
  }

  //This is the input mode where the door is already closed and locked
  //In this mode the user needs to input the password

  else if (safe_door < 10 && mode_input == true) {

    while (counter < 1) {
      digitalWrite(YELLOW_LED, HIGH);
      delay(200);
      digitalWrite(YELLOW_LED, LOW);
      tone(BUZZER, 400, 200);
      counter++;
    }

    //The first button who increment the variable digit in a loop until 5

    current_btn_state = digitalRead(KEY1);
    if ( current_btn_state != last_btn_state )
    {
      delay(100);
      current_btn_state = digitalRead(KEY1);
      if ( current_btn_state == LOW ) {
        digit = digit % 5 + 1;
        tone(BUZZER, 500, 200);
      }
      last_btn_state = current_btn_state;
    }

    //The second button who increment the variable state, which is the printing position in display and resets at the same time the digit to value 1.

    nd_current_btn_state = digitalRead(KEY2);
    if ( nd_current_btn_state != nd_last_btn_state )
    {
      delay(100);
      nd_current_btn_state = digitalRead(KEY2);
      if ( nd_current_btn_state == LOW && digit > 0 )
      {
        state++;
        tone(BUZZER, 200, 200);
        digit = 0;
      }
      nd_last_btn_state = nd_current_btn_state;
    }

    if (nd_current_btn_state == LOW && digit > 0)
    {
      state++;
      digit = 0;
    }

    //Turn on the alram for break in mode

    alarm = true;

    //Printing

    if (digit == 0) {
      Display.showCharAt(state, '-');
    }

    //Get the values from each state and printing


    if (digit > 0) {
      Display.showCharAt(state, '0' + digit);
      if (state == 0)
        nr_1 = digit;
      else if (state == 1)
        nr_2 = digit;
      else if (state == 2)
        nr_3 = digit;
      else if (state == 3)
        nr_4 = digit;
    }
    else if (state == 4)
      code = 1000 * nr_1 + 100 * nr_2 + 10 * nr_3 + nr_4;

    //If the password(variable code) it's equal to the right password then the safe can be open

    if (state == 4 && code == right_code) {
      tone(BUZZER, 800, 500);
      digitalWrite(GREEN_LED, HIGH);
      while (counter < 2) {
        digitalWrite(YELLOW_LED, HIGH);
        delay(200);
        digitalWrite(YELLOW_LED, LOW);
        tone(BUZZER, 400, 200);
        counter++;
      }



      //Reste the value of variables, and enter the base mode

      mode_input = false;
      alarm = false;
      state = 0;
      digit = 0 ;
      error = 0;
      counter = 0;
    }

    //If the password it's not right then repet the input mode loop again(I mean the loop we are in now)

    else if (state == 4 && code != right_code) {
      Display.show(" ");
      tone(BUZZER, 200, 500);
      state = 0;
      error++;

      //If the user inputs the password 3 times wrong in a row then the alarm safe starts

      if (error == 3) {
        Serial.println("ALARM SAFE #");
        for (int i = 0; i < 5; i++) {
          digitalWrite(RED_LED, HIGH);
          tone(BUZZER, 1000);
          delay(500);
          digitalWrite(RED_LED, LOW);
          noTone(BUZZER);
          delay(500);
        }
        error = 0;
      }
    }
  }

  //The break in mode where the safe it's locked but someone opened it without the right password

  else if (safe_door > 30 && mode_input == true && alarm == true) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(RED_LED, HIGH);
      tone(BUZZER, 1000);
      delay(500);
      digitalWrite(RED_LED, LOW);
      noTone(BUZZER);
      delay(500);
    }

    //Reset the value of the variables and go back to the base mode.

    alarm = false;
    state = 0;
    digit = 0;
    error = 0;
    Display.clear();
  }
}
