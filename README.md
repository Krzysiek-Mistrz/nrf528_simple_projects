# NRF528 projects

set of small zephyr apps 4 NRF52840 DK, written while
learning low-level equivalents embed shit 4 zeophyr: ISRs, UART, I2C, SPI, PWM, ADC, & RTOS primitives 4 zephyr. Each project under `projects/` targets one peripheral or concept in isolation as 4 prev uC.... Building upon my lib arc agnostic ie. hardware independent C lib -> `embedkit`

## Structure

```
/embedkit/               embedkit library - submod
/projects/
  blinky/                minimal zephyr app 2 copy & paste
  button_irq/            GPIO interrupt & debounce
  uart_frame_echo/       UART ISR -> ring buffer -> frame parser
  i2c_scan/              I2C bus scan
  spi_loopback/          SPI self-test -> MOSI-MISO jumper, no sensor needed
  pwd_led/               PWM-driven breathing LED
  adc_potentiometer/     SAADC reading via potentiometer
  rtos_threads/          two threads communicating over a k_msgq
```

## Prerequisites

- NRF connect sdk -> zephyr & nordics overlay, installed via NRF connect 4 vscode or manually per nordics documentation.
- `west` available on PATH
- An NRF52840 DK, connected over USB and also it has on board J-Link debugger, so no external programmer needed...

## Cloning

`embedkit` is dep -> git submodule. Clone with:

```bash
git clone --recurse-submodules
```

or, if already cloned without that flag:

```bash
git submodule update --init --recursive
```

## Building and flashing a project

From the west workspace root - the directory containing `.west/`, or wherever your
toolchains build commands expect to run 4m:

```bash
west build -b NRF52840dk/NRF52840 NRF528_project/NRF/projects/<project_name>
west flash
```

Add `-p always` to `west build` 4 clean rebuild when switching between projects.

## Viewing output

- UART-over-USB console: `/dev/ttyACM0` at 115200 baud, e.g. `picocom -b 115200 /dev/ttyACM0`, 4 projects w/ `CONFIG_SERIAL=y`.
- RTT: `west attach`, or the RTT Terminal panel in NRF Connect for VS Code - works even without UART console configured

## embedkit

The shared library lives in `lib/embedkit` as its own repository, included here as submod. It has no Zephyr or hardware dependencies, is unit-tested with GoogleTest, and is pulled into each NRF project unmodified. See `lib/embedkit/README.md` for its own build and usage instructions.

Built with ❤️ using C/Zephyr
Modified MIT License © 2026