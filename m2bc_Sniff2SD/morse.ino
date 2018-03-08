#if 0

/*
  http://www.kent-engineers.com/codespeed.htm
  The word PARIS (".--. .- .-. .. ...") is the standard for determing CW code speed, it is exactly 50units long
  
  dit             = 1
  dah             = 3
  intra-character = 1
  inter-character = 3
  inter-word      = 7
  
  |0        1         2         3         4         5|
  |123456789*123456789*123456789*123456789*123456789*|
  |* *** *** *   * ***   * *** *   * *   * * *       |
  |.|---|---|.   .|---   .|---|.   .|.   .|.|.       |
  |P~~~~~~~~~~___A~~~~___R~~~~~~___I~~___S~~~~.......|

*/

#define LEN_DIT       (1)
#define LEN_DAH       (3)
#define LEN_GAP_TONE  (-1)
#define LEN_GAP_CHAR  (-3)
#define LEN_GAP_WORD  (-7)

#define CODE_ST  (' ')
#define CODE_ND  ('_')

const  char*  mcode[CODE_ND - CODE_ST + 1] =  {  // Array of string pointers
  /*mcode[' '  - CODE_ST] =*/ " "      ,  // | x |   | SPC  | 0x20 = 32 |
  /*mcode['!'  - CODE_ST] =*/ "-.-.--" ,  // |   | ! |      | 0x21 = 33 |
  /*mcode['"'  - CODE_ST] =*/ ".-..-." ,  // |   | " |      |           |
  /*mcode['#'  - CODE_ST] =*/ ""       ,  // | x | # |      |           |
  /*mcode['$'  - CODE_ST] =*/ "...-..-",  // |   | $ |      |           |
  /*mcode['%'  - CODE_ST] =*/ ""       ,  // | x | % |      |           |
  /*mcode['&'  - CODE_ST] =*/ ".-..."  ,  // |   | & | WAIT |           |
  /*mcode['\'' - CODE_ST] =*/ ".----." ,  // |   | \ |      |           |
  /*mcode['('  - CODE_ST] =*/ "-.--."  ,  // |   | ( |      |           |
  /*mcode[')'  - CODE_ST] =*/ "-.--.-" ,  // |   | ) |      |           |
  /*mcode['*'  - CODE_ST] =*/ ""       ,  // | x | * |      |           |
  /*mcode['+'  - CODE_ST] =*/ ".-.-."  ,  // |   | + |      |           |
  /*mcode[','  - CODE_ST] =*/ "--..--" ,  // |   | , |      |           |
  /*mcode['-'  - CODE_ST] =*/ "-....-" ,  // |   | - |      |           |
  /*mcode['.'  - CODE_ST] =*/ ".-.-.-" ,  // |   | . |      |           |
  /*mcode['/'  - CODE_ST] =*/ "-..-."  ,  // |   | / |      |           |
  /*mcode['0'  - CODE_ST] =*/ "-----"  ,  // |   | 0 |      | 0x30 = 48 |
  /*mcode['1'  - CODE_ST] =*/ ".----"  ,  // |   | 1 |      |           |
  /*mcode['2'  - CODE_ST] =*/ "..---"  ,  // |   | 2 |      |           |
  /*mcode['3'  - CODE_ST] =*/ "...--"  ,  // |   | 3 |      |           |
  /*mcode['4'  - CODE_ST] =*/ "....-"  ,  // |   | 4 |      |           |
  /*mcode['5'  - CODE_ST] =*/ "....."  ,  // |   | 5 |      |           |
  /*mcode['6'  - CODE_ST] =*/ "-...."  ,  // |   | 6 |      |           |
  /*mcode['7'  - CODE_ST] =*/ "--..."  ,  // |   | 7 |      |           |
  /*mcode['8'  - CODE_ST] =*/ "---.."  ,  // |   | 8 |      |           |
  /*mcode['9'  - CODE_ST] =*/ "----."  ,  // |   | 9 |      |           |
  /*mcode[':'  - CODE_ST] =*/ "---..." ,  // |   | : |      |           |
  /*mcode[';'  - CODE_ST] =*/ "-.-.-." ,  // |   | ; |      |           |
  /*mcode['<'  - CODE_ST] =*/ "-.-.-"  ,  // | x | < |  SOT |           |
  /*mcode['='  - CODE_ST] =*/ "-...-"  ,  // |   | = |      |           |
  /*mcode['>'  - CODE_ST] =*/ "...-.-" ,  // | x | > |  EOT |           |
  /*mcode['?'  - CODE_ST] =*/ "..--.." ,  // |   | ? |      |           |
  /*mcode['@'  - CODE_ST] =*/ ".--.-." ,  // |   | @ |      |           |
  /*mcode['A'  - CODE_ST] =*/ ".-"     ,  // |   | A |      | 0x41 = 65 |
  /*mcode['B'  - CODE_ST] =*/ "-..."   ,  // |   | B |      |           |
  /*mcode['C'  - CODE_ST] =*/ "-.-."   ,  // |   | C |      |           |
  /*mcode['D'  - CODE_ST] =*/ "-.."    ,  // |   | D |      |           |
  /*mcode['E'  - CODE_ST] =*/ "."      ,  // |   | E |      |           |
  /*mcode['F'  - CODE_ST] =*/ "..-."   ,  // |   | F |      |           |
  /*mcode['G'  - CODE_ST] =*/ "--."    ,  // |   | G |      |           |
  /*mcode['H'  - CODE_ST] =*/ "...."   ,  // |   | H |      |           |
  /*mcode['I'  - CODE_ST] =*/ ".."     ,  // |   | I |      |           |
  /*mcode['J'  - CODE_ST] =*/ ".---"   ,  // |   | J |      |           |
  /*mcode['K'  - CODE_ST] =*/ "-.-"    ,  // |   | K |      |           |
  /*mcode['L'  - CODE_ST] =*/ ".-.."   ,  // |   | L |      |           |
  /*mcode['M'  - CODE_ST] =*/ "--"     ,  // |   | M |      |           |
  /*mcode['N'  - CODE_ST] =*/ "-."     ,  // |   | N |      |           |
  /*mcode['O'  - CODE_ST] =*/ "---"    ,  // |   | O |      |           |
  /*mcode['P'  - CODE_ST] =*/ ".--."   ,  // |   | P |      |           |
  /*mcode['Q'  - CODE_ST] =*/ "--.-"   ,  // |   | Q |      |           |
  /*mcode['R'  - CODE_ST] =*/ ".-."    ,  // |   | R |      |           |
  /*mcode['S'  - CODE_ST] =*/ "..."    ,  // |   | S |      |           |
  /*mcode['T'  - CODE_ST] =*/ "-"      ,  // |   | T |      |           |
  /*mcode['U'  - CODE_ST] =*/ "..-"    ,  // |   | U |      |           |
  /*mcode['V'  - CODE_ST] =*/ "...-"   ,  // |   | V |      |           |
  /*mcode['W'  - CODE_ST] =*/ ".--"    ,  // |   | W |      |           |
  /*mcode['X'  - CODE_ST] =*/ "-..-"   ,  // |   | X |      |           |
  /*mcode['Y'  - CODE_ST] =*/ "-.--"   ,  // |   | Y |      |           |
  /*mcode['Z'  - CODE_ST] =*/ "--.."   ,  // |   | Z |      |           |
  /*mcode['['  - CODE_ST] =*/ ""       ,  // | x | [ |      | 0x5B = 91 |
  /*mcode['\\' - CODE_ST] =*/ ""       ,  // | x | \ |      |           |
  /*mcode[']'  - CODE_ST] =*/ ""       ,  // | x | ] |      |           |
  /*mcode['^'  - CODE_ST] =*/ ".......",  // | x | ^ | ERR  |           |
  /*mcode['_'  - CODE_ST] =*/ "..--.-"    // |   | _ |      | 0x5F = 95 |
}

//----------------------------------------------------------------------------------------------------------------------
int  _cs = 1;
int  _ts = 1;

//+=====================================================================================================================
// https://en.wikipedia.org/wiki/Morse_code#Speed_in_words_per_minute
// "...for the Radiotelegraph Operator License, the examinee must pass a 20 word per minute plain text test..."
//
// https://en.wikipedia.org/wiki/Morse_code#Farnsworth_speed
// "Sometimes, especially while teaching Morse code, the timing rules above are changed so two different speeds are used: 
// a character speed and a text speed. 
// The character speed is how fast each individual letter is sent. The text speed is how fast the entire message is sent. 
// EG. Characters may be sent at 13 wpm, but the intercharacter and interword gaps may be lengthened to 5 wpm.
//
int  moreseSetSpeed (int cs,  int ts)
{
  if (!cs)  cs = 20 ;
  if (!ts)  ts = cs ;
  
  _cs = 1200 / cs;
  _ts = 1200 / ts;
  
  return 0;  // OK
}

//+=====================================================================================================================
int  moreseInit (int cs,  int ts)
{
  pinMode(LED_BUILTIN, OUTPUT);
  (void)morseSetSpeed(cs, ts);
  return 0;  // OK
}

//+=====================================================================================================================
int  morseBeep (int len)
{
  if (len < 0) {
    delay((-len) * _ts);
    
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(len * _cs);
    digitalWrite(LED_BUILTIN, LOW);
    delay((-LEN_GAP_TONE) * _ts);
  }
  return 0;  // OK
}

//+=====================================================================================================================
int  morse (char* msg)
{
  int  ret = 0 ;  // assume success
  
  if (!msg)  return -1 ;
  
  while (*msg) {
    char   c = topupper(*msg) - CODE_ST;

    // Count number of invalid characters
    if ((c < lo) || (c > hi) || (c == '#') || (c == '%') || (c == '*') || (c == '[') || (c == '\\') || (c == ']')) {
      ret++ ;  
      continue;
    }

    for (char*  s = mcode[c];  *s;  s++) {
      switch (s) {
        case ' ' :  morseBeep(LEN_GAP_WORD);  break;
        case '.' :  morseBeep(LEN_DIT     );  break;
        case '-' :  morseBeep(LEN_DAH     );  break;
        default  :  ret++;                    continue;
      }
    }
    morseBeep(LEN_GAP_CHAR);  break;
  }
  
  return ret;
}

#endif

