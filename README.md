# ESP32 Variometer

This project implements a simple variometer using an ESP32, an MS5x series barometer, and an SSD1306 OLED display. The variometer measures pressure to estimate altitude and detects changes of altitude (rising, falling, or stable), providing both visual feedback on the OLED screen and auditory feedback using a speaker.

## Features:

Altitude Tracking: Displays both QFE (altitude difference from a start point) and QNH (altitude difference from sea level).
Altitude Trend Analysis: Detects rising, falling, or stable, and outputs the result on the OLED display.
Sound Feedback: Generates high or low tones based on the altitude trend, allowing for intuitive auditory feedback.

## Hardware Required:

* ESP32 (e.g., Wemos Lolin32)
* MS5x series barometer (e.g., BMP180)
* SSD1306 OLED display
* Speaker for audio feedback

## Libraries Used:

* Wire.h
* Adafruit_GFX.h
* Adafruit_SSD1306.h
* MS5x.h

## How It Works:

Initialization: The program initializes the barometer and OLED display.
Data Collection: Continuously measures temperature, pressure, and calculates altitude.
Altitude Trend: Analyzes recent altitude changes and displays the trend.
Sound Alerts: Produces audio tones to signal altitude rising or falling trends.
