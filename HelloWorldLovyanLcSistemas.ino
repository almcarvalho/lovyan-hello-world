#define LGFX_USE_V1
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <esp32-hal-psram.h>
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_RGB _panel;
  lgfx::Bus_RGB   _bus;
  lgfx::Light_PWM _light;

public:
  LGFX() {
    { // Tamanho lógico
      auto pc = _panel.config();
      pc.memory_width  = 800;
      pc.panel_width   = 800;
      pc.memory_height = 480;
      pc.panel_height  = 480;
      _panel.config(pc);
    }

    { // Barramento RGB – Sunton 8048S070
      auto bc = _bus.config();
      bc.panel = &_panel;

      bc.pin_d0  = 15;  bc.pin_d1  = 7;   bc.pin_d2  = 6;   bc.pin_d3  = 5;
      bc.pin_d4  = 4;   bc.pin_d5  = 9;   bc.pin_d6  = 46;  bc.pin_d7  = 3;
      bc.pin_d8  = 8;   bc.pin_d9  = 16;  bc.pin_d10 = 1;   bc.pin_d11 = 14;
      bc.pin_d12 = 21;  bc.pin_d13 = 47;  bc.pin_d14 = 48;  bc.pin_d15 = 45;

      bc.pin_henable = 41;
      bc.pin_vsync   = 40;
      bc.pin_hsync   = 39;
      bc.pin_pclk    = 42;

      bc.freq_write = 24000000;   // 24 MHz
      bc.hsync_front_porch = 40;  bc.hsync_pulse_width = 48;  bc.hsync_back_porch = 88;
      bc.vsync_front_porch = 13;  bc.vsync_pulse_width = 3;   bc.vsync_back_porch = 32;
      bc.pclk_idle_high = 0;

      _bus.config(bc);
      _panel.setBus(&_bus);
    }

    { // Backlight PWM
      auto lc = _light.config();
      lc.pin_bl = 2;
      lc.invert = false;
      _light.config(lc);
      _panel.light(&_light);
    }

    setPanel(&_panel);
  }
} lcd;

// Desenha fundo branco com um "card" azul centralizado e texto branco
void drawCard() {
  const uint16_t bg = TFT_WHITE;
  const uint16_t card = TFT_BLUE;
  const uint16_t fg = TFT_WHITE;

  lcd.fillScreen(bg);

  const int w = lcd.width();
  const int h = lcd.height();

  // Tamanho do card (ajuste à vontade)
  const int cardW = (int)(w * 0.70f);   // 70% da largura
  const int cardH = (int)(h * 0.25f);   // 25% da altura
  const int x = (w - cardW) / 2;
  const int y = (h - cardH) / 2;
  const int r = min(cardW, cardH) / 10; // raio cantos

  lcd.fillRoundRect(x, y, cardW, cardH, r, card);

  // Texto centralizado dentro do card
  lcd.setTextDatum(lgfx::textdatum::middle_center);
  lcd.setTextFont(4);      // Fonte média
  lcd.setTextSize(2);      // Leve aumento
  lcd.setTextColor(fg, card);
  lcd.drawString("LC SISTEMAS", w / 2, h / 2);
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.setRotation(0);       // Paisagem 800×480
  lcd.setBrightness(255);

  drawCard();
}

void loop() {
  // Tela estática — nada a fazer no loop
}
