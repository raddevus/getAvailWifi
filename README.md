### What It Does
When the ESP32-S3 begins running it will do a one-time scan of the available wifi networks & load them into a array of struct which allows storage of:
1. ssid
2. rssi - distance to wifi
3. channel
4. encryption type (wpa, wpa2, etc.)

If you want to scan again, you need to hit the reset button on the ESP32

### Cycle Over Data
To iterate over the data and see the values stored in the struct on the display, just click your momentary button and each click will advance to the next wifi network found.<br>
When the last network in the array, it will cycle back to the first one. <br>
Again, the wifi scan only runs one time unless you do a reset on the ESP32

### Schematic With OLED (128x64)
<img width="1428" height="1406" alt="image" src="https://github.com/user-attachments/assets/0e088402-c458-4f81-bcad-c9d28368ac76" />
