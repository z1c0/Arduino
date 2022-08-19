#include "esphome.h"

#define DATA_PIN 16

#define DELAY_LEAD  9000
#define DELAY_SPACE 4500
#define DELAY_BURST  560
#define DELAY_0 DELAY_BURST
#define DELAY_1 1690

#define BTN_PLUS         0
#define BTN_MINUS        1
#define BTN_OFF          2
#define BTN_ON           3
#define BTN_RED          4 // (255,0,0)
#define BTN_GREEN        5 // (0,255,0)
#define BTN_BLUE         6 // (0,0,255)
#define BTN_WHITE        7 // (255,255,255)
#define BTN_ORANGE       8 // (255,165,0)
#define BTN_9            9
#define BTN_10          10
#define BTN_FLASH       11
#define BTN_12          12
#define BTN_13          13
#define BTN_14          14
#define BTN_STROBE      15
#define BTN_16          16
#define BTN_17          17
#define BTN_PURPLE      18 // (128,0,128) 
#define BTN_FADE        19
#define BTN_YELLOW      20 // (255,255,0)
#define BTN_CYAN        21 // (0,255,255)
#define BTN_MAGENTA     22 // (255,0,255)
#define BTN_SMOOTH      23

struct ColorCommand
{
  int Code;
  int R;
  int G;
  int B;
};

ColorCommand colorCommands[] =
{
  { BTN_RED, 255, 0, 0 },
  { BTN_RED, 255, 0, 0 },
  { BTN_GREEN, 0, 255, 0 },
  { BTN_BLUE, 0, 0, 255 },
  { BTN_WHITE, 255, 255, 255 },
  { BTN_ORANGE, 255, 165, 0 },
  { BTN_PURPLE, 128, 0, 128 },
  { BTN_YELLOW, 255, 255, 0 },
  { BTN_CYAN, 0, 255, 255 },
  { BTN_MAGENTA, 255, 0, 255 },
};

class FakeIrLed : public Component, public LightOutput
{
public:
  void setup() override
  {
    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(DATA_PIN, HIGH);
  }

  LightTraits get_traits() override
  {
    // return the traits this light supports
    auto traits = LightTraits();
    traits.set_supported_color_modes({ColorMode::RGB});
    return traits;
  }

  void write_state(LightState *state) override
  {
    float red, green, blue;
    state->current_values_as_rgb(&red, &green, &blue);

    ESP_LOGI("Fake IR LED", "Color: %f, %f, %f", red, green, blue);
    int r = ceil(255 * red);
    int g = ceil(255 * green);
    int b = ceil(255 * blue);
    ESP_LOGI("Fake IR LED", "RGB: %d, %d, %d", r, g, b);

    if (r < 2 && g < 2 && b < 2)
    {
      send(BTN_OFF);
    }
    else
    {
      send(BTN_ON);
      auto color = mapColor(r, g, b);
      send(color);
    }
  }

private:
  uint8_t mapColor(int r, int g, int b)
  {
    uint8_t cmd = 0;
    int minDiff = 10000000;  
    for (int i = 0; i < sizeof(colorCommands) / sizeof(ColorCommand); i++)
    {
      auto diff = 
        abs(r - colorCommands[i].R) + 
        abs(g - colorCommands[i].G) + 
        abs(b - colorCommands[i].B);
      if (diff < minDiff)
      {
        minDiff = diff;
        cmd = colorCommands[i].Code;
      }
    }
    return cmd;
  }

  void send(uint8_t code)
  {
    digitalWrite(DATA_PIN, LOW);
    delayMicroseconds(DELAY_LEAD);
    digitalWrite(DATA_PIN, HIGH);
    delayMicroseconds(DELAY_SPACE);

    send_byte(0x00);
    send_byte(0xEF);

    send_byte(code);
    send_byte(~code);

    digitalWrite(DATA_PIN, LOW);
    delayMicroseconds(DELAY_BURST);

    digitalWrite(DATA_PIN, HIGH);
    delay(200);
  }

  void send_0()
  {
    digitalWrite(DATA_PIN, LOW);
    delayMicroseconds(DELAY_BURST);
    digitalWrite(DATA_PIN, HIGH);
    delayMicroseconds(DELAY_0);
  }

  void send_1()
  {
    digitalWrite(DATA_PIN, LOW);
    delayMicroseconds(DELAY_BURST);
    digitalWrite(DATA_PIN, HIGH);
    delayMicroseconds(DELAY_1);
  }

  void send_byte(uint8_t c)
  {
    for (int bits = 0; bits < 8; bits++)
    {
      if (c & (1 << bits))
      {
        send_1();
      }
      else
      {
        send_0();
      }
    }
  }
};