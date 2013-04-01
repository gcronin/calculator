#include <Keypad.h>
#include <LiquidCrystal.h>
#include <stdlib.h>
#include <math.h>

///////////////////////////////////////////////////////VARIABLES///////////////////////////////////////////
//VARIABLES TO PARSE FLOATING ANSWER INTO WHOLE NUMBER AND INTEGER PARTS///
char outresult[20];
String OutResult;
int decimal_location;
String whole_number;
String decimal_number;

//VARIABLES USED TO SCROLL TOP LINE//
long lasttime =0;
String totalarray = "";  //stores what was input
int length = 0; //length of what was input
int length2 = 0;  //length after the first input is added
bool scroll1 = 0;  //whether to scroll on line one or not
bool scroll2 = 0;  //whether to scroll on line two or not
int scroll_index = 0;

//INPUT VARIABLES//
char input[9]; //buffer for input characters
float firstnumber = 0; //stores first number
float secondnumber = 0;  //stores second number
float response = 0;  //stores "answer"
bool positive = 1; //indicates positive number
int lcdcolumnindexrow1 = 1; // used to indicate location of input key

//FUNCTION VARIABLES//
int function = 9;  //stores currently selected function
const int number_of_functions = 10;
int mode = 2; // select normal operation (1) or mystery function (0)
bool toggle = 1;

///////////////////////////////////////////////////////KEYPAD SETUP///////////////////////////////////////
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'A','3','2','1'},
  {'B','6','5','4'},
  {'C','9','8','7'},
  {'D','#','0','*'}
};
 
  // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 1, 12, 16, 15 };
// Connect keypad COL0, COL1, COL2, and COL3 to these Arduino pins.
byte colPins[COLS] = { 17, 18, 19, 14 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

///////////////////////////////////////////////////////LCD SETUP///////////////////////////////////////
LiquidCrystal lcd(2, 3, 13, 8, 9, 10, 11, 4, 5, 6, 7);

void setup()
{
  lcd.begin(8,2);  //8 characters times 2 rows
  for(int i = 0; i < 8; i++) { input[i] = '0'; }  // initialize input as zero
}


void loop()
{
  while(mode > 1) //setup process:  Allow the user to chose Calculator Mode or Game Mode.  This only runs once at the beginning
  {
    if(millis() - lasttime > 1000) // do this only every 1000 milliseconds.
    {
      if(toggle)  // Display text "Mode"
      {
        lcd.setCursor(0,0);
        lcd.print("MODE?   ");
        lcd.setCursor(0,1);
        lcd.print("        ");
        lasttime = millis();
        toggle = 1-toggle;
      }
      else // Display text "A = Calc"  "B = Game"
      {
        lcd.setCursor(0,0);
        lcd.print("A = Calc");
        lcd.setCursor(0,1);
        lcd.print("B = Game");
        lasttime = millis();
        toggle = 1-toggle;
      }
    }
    char key = kpd.getKey(); // read keypad
    if (key != NO_KEY)
    {
      if(key == 'A') 
      { 
        mode = 1;
        lcd.clear();
      }
      if(key == 'B') 
      { 
        mode = 0;
        function = 10;
        lcd.clear();
       }  
    }
  }
  
  if(scroll1) // display the input number and function(s) on the top line if math has been done
  {
    scrollline(750, 0, length);
  }
  else if(scroll2) // display the input number and function(s) on the second line if awaiting a 2nd number
  {
    scrollline(750, 1, length2);
  }

  char key = kpd.getKey();   // read keypad
  if (key != NO_KEY)
  {
    if(key == 'A') //increment function
    {
      if(mode) 
      {
        function = mod(++function, number_of_functions);
        displayfunction(function);
      }
      else {function = 10;}
    }
    else if(key == 'B') //decrement function
    {
      if(mode) 
      {
        function = mod(--function, number_of_functions);
        displayfunction(function);
      }
      else {function = 10;}
    }
    else if(key == 'C')  //clear
    { 
      lcd.clear();
      for(int i = 0; i < 8; i++) { input[i] = '0'; }
      lcdcolumnindexrow1 = 1;
      firstnumber = 0;
      secondnumber = 0;
      totalarray = "";
      scroll1 = 0;
      scroll2 = 0;
      length = 0;
      length2 = 0;
      if(!mode) {function = 10;} else {function = 9;}
      positive = 1;
    }
    else if(key == 'D')  //enter
    { 
      if(function > 4 && function < 9 || function == 10) //these functions require only one number
      {
        firstnumber = atof(input);  // convert input character array to a float
        numintototalarray();
        printoperation(function);
        response = executefunction(function, firstnumber, 1);  // do the math
        scroll1 = 1;  // display the input number and function on the top line
        lcd.setCursor(0,1);
        lcd.print("        ");  //clear line
        lcd.setCursor(0,1);
        printresponse(response);  //print answer
      }
      else if(function == 9) // no function has been selected... print error message
      {
        lcd.setCursor(0,1);
        lcd.print("FUNCT??");  //error message
      }
      else if(firstnumber == 0)  // this is the first time that enter has been pressed and we have a valid function
      {
        firstnumber = atof(input);  // convert input character array to a float
        numintototalarray();
        printoperation(function);
        lcd.setCursor(0,0);
        lcd.print("        ");  //clear first line to make space for the second number
        lcd.setCursor(0,1);
        lcd.print("        ");  //clear line
        length2 = length;
        scroll2 = 1; //print what the user has entered so far on the second line
        for(int i = 0; i < 8; i++) { input[i] = '0'; } // reinitialize the input buffer to make space for the second number
        lcdcolumnindexrow1 = 1;
        positive = 1;
      }
      else
      {
        secondnumber = atof(input);  // convert input character array to a float
        response = executefunction(function, firstnumber, secondnumber);  // do the math
        numintototalarray();
        totalarray = totalarray + "= ";
        length = length + 2;
        scroll1 = 1;  // display the input numbers and function on the top line
        lcd.setCursor(0,1);
        lcd.print("        ");  //clear line
        lcd.setCursor(0,1);
        printresponse(response);  //print answer
        positive = 1;
      } 
    }
    else if(key == '#' && positive ==1) //negative number read and number is currently positive
    {
      input[0] = '-';
      lcd.setCursor(0,0);
      lcd.print(input[0]); //print negative sign       
      positive = 0;  //indicate number is now negative
      ++length;
    }
    else if(key == '#' && positive ==0)
    {
      positive = 1;  //indicate number is now negative
      input[0] = '0';          
      lcd.setCursor(0,0);
      lcd.print(' '); //remove negative sign
      --length;
    }
    else if(lcdcolumnindexrow1 < 8) //buffer overrun prevention
    {    
         //read in a number or a decimal
        {
          for(int i =1; i < 7; i++) {input[i] = input[i+1]; }  //shift all the values left
          if(key == '*') { input[7] = '.'; }  else {  input[7] = key;  } //* is  decimal
          lcd.setCursor(lcdcolumnindexrow1,0);  //print on left of LCD moving right
          lcd.print(input[7]);
          ++lcdcolumnindexrow1;
          ++length;
        }
      }    
   }  
}

/////////////////////Function Returns Modulo Even for Negative Numbers//////////////////////////
int mod(int a, int b) 
{
  return (((a % b) + b) % b);
}

//////////////////////Function Displays Math Functions/////////////////////////////////////////
void displayfunction(int _function)  
{
    lcd.setCursor(0,1);  
    switch (_function) 
    {
    case 0: 
      lcd.print("+       ");
      break;
    case 1:
      lcd.print("-       ");
      break;
    case 2:
      lcd.print("x       ");
      break;
    case 3:
      lcd.print("/       ");
      break;
    case 4:
      lcd.print("^       ");
      break;
    case 5:
      lcd.print("^2      ");
      break;
    case 6:
      lcd.print("sin     ");
      break;
    case 7:
      lcd.print("cos     ");
      break;
    case 8:
      lcd.print("tan     ");
      break;
    case 9:
      lcd.print("        ");
      break;
    case 10:
      lcd.print("?UGuess?");
      break;
    }
}

//////////////////////Function Decides Whether Output Has 0, 1, 2, 3, or 4 Decimals AND Prints Output/////////////////
void printresponse(float _response)
{
  if(abs(_response) < 0.0001)  {lcd.print("TooSmall");}
  else if(_response > 99999999)  {lcd.print("Too Big ");}
  else if(_response < -9999999) {lcd.print("-Too Big");}
  else
  {
    dtostrf(_response,7,4,outresult);  //Convert Float to Character Array
    for(int i=0; i<19; i++) //Find the location of the decimal
    {
      if(outresult[i] == '.')
      {
        decimal_location = i;
      }  
    }
    OutResult = outresult;  //Convert Character Array to String
    whole_number = OutResult.substring(0,decimal_location);  //Read up to the Decimal Location
    decimal_number = OutResult.substring(decimal_location+1, decimal_location+5);  //Read After the Decimal Location
    if(decimal_number[3] =='0' && decimal_number[2] =='0' && decimal_number[1] =='0' && decimal_number[0] =='0')   //print only the whole number part 
    {
      lcd.print(whole_number);
    }  
    else if(decimal_number[3] =='0' && decimal_number[2] =='0' && decimal_number[1] =='0') // print one decimal place
    {
      OutResult = whole_number + '.' + decimal_number[0];
      lcd.print(OutResult);
    }
    else if(decimal_number[3] =='0' && decimal_number[2] =='0') // print two decimal places
    {
      OutResult = whole_number + '.' + decimal_number[0] + decimal_number[1];
      lcd.print(OutResult);
    }
    else if(decimal_number[3] =='0')  // print three decimal places
    {
      OutResult = whole_number + '.' + decimal_number[0] + decimal_number[1] + decimal_number[2];
      lcd.print(OutResult);
    }
    else {lcd.print(OutResult);}  // print four decimal places
  }
}

//////////////////////FUNCTION ADDS "+" "-" "/" TO STRING ARRAY/////////////////////////////////////////  
void printoperation(int _function)
{
  switch (_function) 
    {
    case 0: 
      totalarray = totalarray + "+";
      length = length + 1;
      break;
    case 1:
      totalarray = totalarray + "-";
      length = length + 1;
      break;
    case 2:
      totalarray = totalarray + "x";
      length = length + 1;
      break;
    case 3:
      totalarray = totalarray + "/";
      length = length + 1;
      break;
    case 4:
      totalarray = totalarray + "^";
      length = length + 1;
      break;
    case 5:
      totalarray = totalarray + "^2= ";
      length = length + 4;
      break;
    case 6:
      totalarray = "sin(" + totalarray + ")= "; 
     length = length + 7; 
      break;
    case 7:
      totalarray = "cos(" + totalarray + ")= ";
      length = length + 7;   
      break;
    case 8:
      totalarray = "tan(" + totalarray + ")= ";
      length = length + 7;   
      break;
    case 9:
      break;
    case 10:
      totalarray = "f(" + totalarray + ")= ";
      length = length + 5;
      break;
    }
}

//////////////////////FUNCTION DOES MATH/////////////////////////////////////////
float executefunction(int _function, float _firstnumber, float _secondnumber)  
{
    float _output;
    switch (_function) 
    {
    case 0: 
      _output = _firstnumber + _secondnumber;
      break;
    case 1:
      _output = _firstnumber - _secondnumber;
      break;
    case 2:
      _output = _firstnumber * _secondnumber;
      break;
    case 3:
      if(_secondnumber != 0) {_output = _firstnumber / _secondnumber;}
      break;
    case 4:
      _output = pow(_firstnumber, _secondnumber);
      break;
    case 5:
      _output = pow(_firstnumber, 2);
      break;
    case 6:
      _output = sin(_firstnumber);
      break;
    case 7:
      _output = cos(_firstnumber); 
      break;
    case 8:
      _output = tan(_firstnumber); 
      break;
    case 9:
      break;
    case 10:
      _output = _firstnumber * _firstnumber - 1;
      break;
    }
    return _output;
}

///////////////////Function Scrolls a Line////////////////////////////////
void scrollline(int delaytime, int line_number, int _length)
{
  if(length > 8) // if the input is too long to fit on the screen, then scroll
  {
    if(millis() - lasttime > delaytime)  // do this only every 750 milliseconds.
    {
      for(int i =0; i<8; i++)
      {
        lcd.setCursor(i,line_number);
        if(scroll_index+i < _length) {lcd.print(totalarray.charAt(scroll_index+i));}
      }
      scroll_index = (scroll_index + 1)%(_length-6);
      lasttime = millis();
    }
  }
  else  // if the input is shorter than 8 characters, no scroll is required
  {
    lcd.setCursor(0,line_number);
    lcd.print(totalarray);
  }
}

////////////////////////////////////////Function Scans Input Number into String Total Array/////////////////////////
void numintototalarray()
{
  if(positive == 0) { totalarray = totalarray + '-'; }
  for(int i = 1; i < 8; i++)
  {
    if(i > (8 - lcdcolumnindexrow1))
    {
      totalarray = totalarray + input[i];
    }
  }
}
