// IMPORTANT: Elegoo_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 40
#define BUTTON_Y 100
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 10
#define BUTTON_SPACING_Y 10
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 5
#define TEXT_Y 5
#define TEXT_W 160
#define TEXT_H 85
#define TEXT_TSIZE 7
#define TEXT_TCOLOR ILI9341_WHITE
// the data (phone #) we store in the textfield
#define TEXT_LEN 3
char textfield[TEXT_LEN + 1] = "";
uint8_t textfield_i=0;

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 900 //920
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65



Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;

Elegoo_GFX_Button buttons[6];
Elegoo_GFX_Button buttons_confirmacion[2];
/* create 15 buttons, in classic candybar phone style */
char buttonlabels[6][9] = {"+", "-", "x10", "Comenzar", "Parar", "Luz" };
uint16_t buttoncolors[6] = {ILI9341_RED, ILI9341_RED, ILI9341_BLUE, 
                             ILI9341_GREEN, ILI9341_RED, ILI9341_WHITE};
                             
int t_exponer;
bool por10 = false;
int ms_to_s = 1000;
unsigned long inicio = 0;
unsigned long fin = 0;
bool esperandoConfirmacion = false;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
   
  }

  tft.begin(identifier);
  tft.setRotation(2);

  //PIN Ampliadora
  pinMode(A5, OUTPUT);
  digitalWrite(A5, HIGH);

  pantallaControladora ();
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// the loop function runs over and over again forever
void loop() {

/*TSPoint p;
  p = ts.getPoint(); 
  */
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  
 // p = ts.getPoint(); 
  /*
  if (ts.bufferSize()) {
    
  } else {
    // this is our way of tracking touch 'release'!
    p.x = p.y = p.z = -1;
  }*/
  
  // Scale from ~0->4000 to tft.width using the calibration #'s
  /*
  if (p.z != -1) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    Serial.print("("); Serial.print(p.x); Serial.print(", "); 
    Serial.print(p.y); Serial.print(", "); 
    Serial.print(p.z); Serial.println(") ");
  }*/
   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

      //Serial.print("Ancho: ");Serial.print(tft.width());Serial.print(" - Alto: ");Serial.println(tft.height());
      //Serial.print("X: ");Serial.print(p.x);Serial.print(" - Y: ");Serial.println(p.y);
      
      // scale from 0->1023 to tft.width
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));

      //Serial.println("TRADUCIDO");
      //Serial.print("X: ");Serial.print(p.x);Serial.print(" - Y: ");Serial.println(p.y);
   }

  if(!esperandoConfirmacion) {
    // go thru all the buttons, checking if they were pressed
    for (uint8_t b=0; b<6; b++) {
      //Serial.print("Botón ");Serial.print(b);Serial.print(": ");
      if (buttons[b].contains(p.x, p.y)) {
        //Serial.print("X: ");Serial.print(p.x);Serial.print(" - Y: ");Serial.println(p.y);
        //Serial.print("Pressing: "); Serial.println(b);
        buttons[b].press(true);  // tell the button it is pressed
      } else {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }
    }
  
  
    // now we can ask the buttons if their state has changed
    for (uint8_t b=0; b<6; b++) {
      if (buttons[b].justReleased()) {
        // Serial.print("Released: "); Serial.println(b);
        buttons[b].drawButton();  // draw normal
      }
      
      if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert!
        
        // Boton +
        if (b == 0) {
          Serial.println("Boton +");
          if(por10) {
            t_exponer = t_exponer + 10;
          } else {
            t_exponer++;
          }
          actualizarPantalla(t_exponer);
        }

        // Boton -
        if (b == 1) {
          Serial.println("Boton -");
          if(por10) {
            Serial.println("x10 ACTIVO");
            if (t_exponer > 10) {
              t_exponer = t_exponer - 10;
            }
          } else {
            Serial.println("x10 DESACTIVADO");
            if (t_exponer > 1) {
              t_exponer--;
            }
          }
          actualizarPantalla(t_exponer);
        }

        // Boton x10
        if (b == 2) {
          Serial.println("Boton X10");
          if(por10) {
            Serial.println("x10 ACTIVO");
          } else {
            Serial.println("x10 DESACTIVADO");
          }
          por10 = !por10;
        }

        if (b == 3) {
          Serial.println("Boton Comenzar");
          esperandoConfirmacion = true;
          pantallaConfirmarExposicion ();
          actualizarContador(t_exponer);
        }
        
        if (b == 4) {
          Serial.println("Boton Parar");
          
          digitalWrite(A5, HIGH);

        }
        
        if (b == 5) {
          Serial.println("Boton Luz");
          
          digitalWrite(A5, LOW);

        }
      }
    }
  } else {
    // go thru all the buttons, checking if they were pressed
    for (uint8_t b=0; b<2; b++) {
      if (buttons_confirmacion[b].contains(p.x, p.y)) {
        buttons_confirmacion[b].press(true);  // tell the button it is pressed
      } else {
        buttons_confirmacion[b].press(false);  // tell the button it is NOT pressed
      }
    }
  
    for (uint8_t b=0; b<2; b++) {
      if (buttons_confirmacion[b].justReleased()) {
        buttons_confirmacion[b].drawButton();  // draw normal
      }
      if (buttons_confirmacion[b].justPressed()) {
        buttons_confirmacion[b].drawButton(true);  // draw invert!
        
        // Boton Sí
        if (b == 0) {
          exponer(t_exponer);
        }
        // Boton No
        if (b == 1) {
          esperandoConfirmacion = false;
          pantallaControladora ();
        }
      }
    }
  }
  
  delay(10); // UI debouncing
}

void exponer(int tiempo) {
  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(A5, LOW);

  inicio = millis();
  fin = millis();
  while ((fin - inicio) / ms_to_s < tiempo) {
    
    fin = millis();

    if((fin - inicio) % ms_to_s == 0) {
      Serial.print("Quedan: ");
      Serial.println(tiempo - ((fin - inicio) / ms_to_s));

      actualizarContador(tiempo - ((fin - inicio) / ms_to_s));
      
    }
  }

  digitalWrite(A5, HIGH);
  
  digitalWrite(LED_BUILTIN, LOW);

  delay(1000);
  
  esperandoConfirmacion = false;
  pantallaControladora ();
}

void actualizarPantalla(int valor) {
  // update the current text field
  Serial.print("Actualizar Pantalla: ");Serial.println(valor);
  tft.setCursor(TEXT_X + 12, TEXT_Y + 18);
  tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft.setTextSize(TEXT_TSIZE);

  sprintf(textfield,"%3d", valor);
  tft.print(textfield);
}

void actualizarContador(int valor) {
  // update the current text field
  Serial.print("Actualizar Contador: ");Serial.println(valor);
  tft.setCursor(25, 100);
  tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft.setTextSize(TEXT_TSIZE);

  sprintf(textfield,"%3d", valor);
  tft.print(textfield);
}

void pantallaControladora () {
  tft.fillScreen(BLACK);
  
  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);

  // create buttons
  // Boton +
  buttons[0].initButton(&tft, 60, 
                              125,    // x, y, w, h, outline, fill, text
                              110, 
                              55, 
                              ILI9341_BLACK, 
                              ILI9341_RED, 
                              ILI9341_WHITE,
                              "+", 
                              BUTTON_TEXTSIZE); 
  buttons[0].drawButton();
  
  // Boton -
  buttons[1].initButton(&tft, 180, 
                              125,    // x, y, w, h, outline, fill, text
                              110, 
                              55, 
                              ILI9341_BLACK, 
                              ILI9341_RED, 
                              ILI9341_WHITE,
                              "-", 
                              BUTTON_TEXTSIZE); 
  buttons[1].drawButton();

  // Boton -
  buttons[2].initButton(&tft, 200, 
                              47,    // x, y, w, h, outline, fill, text
                              60, 
                              TEXT_H, 
                              ILI9341_BLACK, 
                              ILI9341_BLUE, 
                              ILI9341_WHITE,
                              "X10", 
                              BUTTON_TEXTSIZE); 
  buttons[2].drawButton();

  // Boton Comenzar
  buttons[3].initButton(&tft, 120, 
                              195,    // x, y, w, h, outline, fill, text
                              230, 
                              55, 
                              ILI9341_BLACK, 
                              ILI9341_GREEN, 
                              ILI9341_WHITE,
                              "Comenzar", 
                              BUTTON_TEXTSIZE); 
  buttons[3].drawButton();

  // Boton Parar
  buttons[4].initButton(&tft, 120, 
                              255,    // x, y, w, h, outline, fill, text
                              230, 
                              55, 
                              ILI9341_BLACK, 
                              ILI9341_BLUE, 
                              ILI9341_WHITE,
                              "Parar", 
                              BUTTON_TEXTSIZE); 
  buttons[4].drawButton();

  // Boton Luz
  buttons[5].initButton(&tft, 120, 
                              305,    // x, y, w, h, outline, fill, text
                              230, 
                              35, 
                              ILI9341_BLACK, 
                              ILI9341_BLACK, 
                              ILI9341_WHITE,
                              "Luz", 
                              BUTTON_TEXTSIZE); 
  buttons[5].drawButton();

  t_exponer = 5;
  actualizarPantalla(t_exponer);
}

void pantallaConfirmarExposicion () {
  tft.fillScreen(BLACK);
  
  // create buttons
  // Boton +
  buttons_confirmacion[0].initButton(&tft, 60, 
                              290,    // x, y, w, h, outline, fill, text
                              110, 
                              55, 
                              ILI9341_BLACK, 
                              ILI9341_BLACK, 
                              ILI9341_RED,
                              "Si", 
                              BUTTON_TEXTSIZE); 
  buttons_confirmacion[0].drawButton();
  
  // Boton -
  buttons_confirmacion[1].initButton(&tft, 180, 
                              290,    // x, y, w, h, outline, fill, text
                              110, 
                              55, 
                              ILI9341_BLACK, 
                              ILI9341_BLACK, 
                              ILI9341_RED,
                              "No", 
                              BUTTON_TEXTSIZE); 
  buttons_confirmacion[1].drawButton();

}
