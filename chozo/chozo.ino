#include <Adafruit_NeoPixel.h>

#define LEDS_PIN    4
#define NUM_LEDS    1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);

int brightness = 0;
int step = 5;

void setup() 
{
  strip.begin();
}

void loop() 
{
  brightness += step;

  if (brightness < 0)
  {    
    brightness = 0;
    step *= -1;
  }
  else if (brightness > 255)
  {    
    brightness = 255;
    step *= -1;
  }

  strip.setBrightness(brightness);
  strip.setPixelColor(0, 0, 240, 245);
  strip.show();

  delay(150);
}


