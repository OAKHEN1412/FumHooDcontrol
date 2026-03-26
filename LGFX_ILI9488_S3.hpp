#pragma once
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9488 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX() {
    {   // ---------- SPI BUS ----------
        auto cfg = _bus.config();

        cfg.spi_host = SPI2_HOST;     // ESP32-S3 FSPI
        cfg.spi_mode = 0;
        cfg.freq_write = 40000000;
        cfg.freq_read  = 16000000;
        cfg.spi_3wire = true;
        cfg.use_lock  = true;
        cfg.dma_channel = 1;

        // ---- PIN ของคุณ (เปลี่ยนได้) ----
        cfg.pin_sclk = 12;
        cfg.pin_mosi = 11;
        cfg.pin_miso = -1;
        cfg.pin_dc   = 17;

        _bus.config(cfg);
        _panel.setBus(&_bus);
    }

    {   // ---------- PANEL ILI9488 ----------
        auto cfg = _panel.config();

        cfg.pin_cs   = 10;
        cfg.pin_rst  = 18;
        cfg.pin_busy = -1;

        cfg.panel_width  = 320;
        cfg.panel_height = 480;
        cfg.memory_width  = 320;
        cfg.memory_height = 480;

        cfg.readable = false;
        cfg.invert = false;
        cfg.rgb_order = false;

        cfg.dummy_read_pixel = 8;
        cfg.bus_shared = true;

        _panel.config(cfg);
    }

    setPanel(&_panel);
  }
};
