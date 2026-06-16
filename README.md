# 💊 RFID-Based Medicine Reminder and Smart Medicine Box

An Arduino-based automated medication management system that reminds users to take their medicine on schedule, grants access to the medicine box via RFID authentication, and automatically re-locks the box after use.

> **Course:** CPEE Cognate Track Course 1
> **Instructor:** Engr. Joseth Angelo A. Bidding, CCpE
> **Institution:** Notre Dame of Tacurong College — College of Arts, Sciences and Education

-----

## 📋 Features

- 🕐 **Real-Time Clock Display** — Shows the current time on the LCD continuously
- 💡 **Color-Coded LED Alerts** — Five LEDs assigned to five medication time slots (Red, Green, Blue, Orange, Purple)
- 🔔 **Buzzer Alarm** — Sounds at each scheduled medication time to alert the user
- 🔐 **RFID Authentication** — Only the correct RFID card unlocks the medicine box
- ⚙️ **Servo Motor Lock** — Physically locks and unlocks the medicine compartment
- ⏱️ **Auto Re-lock** — Automatically locks the box 5 seconds after access is granted
- ❌ **Access Denied** — Unrecognized RFID cards are rejected and displayed on the LCD
- ⚠️ **Missed Dose Alert** — If the user doesn’t respond within 10 seconds, the LCD shows a missed medication message

-----

## 🛠️ Hardware Components

|Component                           |Quantity|
|------------------------------------|--------|
|Arduino UNO                         |1       |
|MFRC522 RFID Reader Module          |1       |
|RFID Card / Tag                     |1       |
|DS3231 Real-Time Clock Module       |1       |
|16x2 I2C LCD Display (Address: 0x27)|1       |
|Servo Motor (SG90 or equivalent)    |1       |
|Buzzer                              |1       |
|Red LED                             |1       |
|Green LED                           |1       |
|Blue LED                            |1       |
|RGB LED (for Orange — pins R+G)     |1       |
|RGB LED (for Purple — pins R+B)     |1       |
|220Ω Resistors                      |7       |
|Breadboard                          |1       |
|Jumper Wires                        |Several |

-----

## 🔌 Pin Connections

|Component          |Component Pin|Arduino Pin        |Purpose         |
|-------------------|-------------|-------------------|----------------|
|MFRC522 RFID Reader|SDA (SS)     |Digital Pin 10     |SPI Chip Select |
|MFRC522 RFID Reader|SCK          |Digital Pin 13     |SPI Clock       |
|MFRC522 RFID Reader|MOSI         |Digital Pin 11     |SPI Data Out    |
|MFRC522 RFID Reader|MISO         |Digital Pin 12     |SPI Data In     |
|MFRC522 RFID Reader|RST          |Analog Pin A3      |Reset Signal    |
|MFRC522 RFID Reader|3.3V         |3.3V               |Power           |
|MFRC522 RFID Reader|GND          |GND                |Ground          |
|DS3231 RTC Module  |SDA          |Analog Pin A4      |I2C Data        |
|DS3231 RTC Module  |SCL          |Analog Pin A5      |I2C Clock       |
|DS3231 RTC Module  |VCC          |5V                 |Power           |
|DS3231 RTC Module  |GND          |GND                |Ground          |
|16x2 LCD (I2C)     |SDA          |Analog Pin A4      |I2C Data        |
|16x2 LCD (I2C)     |SCL          |Analog Pin A5      |I2C Clock       |
|16x2 LCD (I2C)     |VCC          |5V                 |Power           |
|16x2 LCD (I2C)     |GND          |GND                |Ground          |
|Servo Motor        |Signal       |Digital Pin 8 (PWM)|PWM Control     |
|Servo Motor        |VCC          |5V                 |Power           |
|Servo Motor        |GND          |GND                |Ground          |
|Buzzer             |Positive (+) |Analog Pin A0      |Alert Tone      |
|Buzzer             |Negative (−) |GND                |Ground          |
|Red LED (Slot 1)   |Anode (+)    |Digital Pin 2      |Alert Indicator |
|Green LED (Slot 2) |Anode (+)    |Digital Pin 3      |Alert Indicator |
|Blue LED (Slot 3)  |Anode (+)    |Digital Pin 4      |Alert Indicator |
|Orange LED (Slot 4)|Anode R (+)  |Digital Pin 5      |Red Channel     |
|Orange LED (Slot 4)|Anode G (+)  |Digital Pin 6      |Green Channel   |
|Purple LED (Slot 5)|Anode R (+)  |Digital Pin 7      |Red Channel     |
|Purple LED (Slot 5)|Anode B (+)  |Digital Pin 9      |Blue Channel    |
|All LEDs           |Cathode (−)  |GND via 220Ω       |Current Limiting|


> **Note:** The DS3231 RTC and 16x2 I2C LCD share the same I2C bus (A4 = SDA, A5 = SCL). This is normal and supported.

-----

## 📦 Required Libraries

Install these libraries via the Arduino IDE Library Manager (`Sketch > Include Library > Manage Libraries`):

|Library                                  |Purpose                       |
|-----------------------------------------|------------------------------|
|`MFRC522` by GithubCommunity             |RFID reader communication     |
|`RTClib` by Adafruit                     |DS3231 RTC timekeeping        |
|`LiquidCrystal_I2C` by Frank de Brabander|I2C LCD control               |
|`Servo`                                  |Servo motor control (built-in)|
|`SPI`                                    |SPI communication (built-in)  |
|`Wire`                                   |I2C communication (built-in)  |

-----

## 🚀 Setup & Installation

1. **Clone or download** this repository
   
   ```
   git clone https://github.com/[your-username]/rfid-medicine-box.git
   ```
1. **Open** the `.ino` file in the Arduino IDE
1. **Install all required libraries** listed above via the Library Manager
1. **Set the RTC time** — On first upload, the following line in `setup()` will automatically set the RTC to your computer’s current time:
   
   ```cpp
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   ```

> ⚠️ **Important:** After the first successful upload, **comment out this line** and re-upload to prevent the time from resetting every time the Arduino restarts.
1. **Find your RFID card’s UID:**
- Upload the sketch and open the Serial Monitor (baud rate: 9600)
- Tap your RFID card on the reader
- The UID will be printed in the Serial Monitor (e.g., `UID: 123 45 67 89`)
- Update the `allowedUID[]` array in the code with your card’s actual UID:
  
  ```cpp
  byte allowedUID[] = {0x7B, 0x2D, 0x43, 0x59}; // Replace with your UID
  ```
1. **Upload** the final sketch to your Arduino UNO
1. **Power on** — The LCD will display “Smart Medicine System Ready” and then begin showing the current time

-----

## ⚙️ How It Works

```
[Power On]
     ↓
Display Current Time on LCD
     ↓
Every 30 seconds → Medication Alert Triggers
     ↓
Corresponding LED lights up + Buzzer sounds
     ↓
     ├── Correct RFID Card Tapped?
     │        ↓ YES
     │   Buzzer stops → Servo unlocks box
     │   LCD: "Welcome Kurt! Time for your Medicine!"
     │        ↓
     │   5-second window → Servo re-locks → LED off
     │        ↓
     │   Back to displaying current time
     │
     ├── Wrong RFID Card Tapped?
     │        ↓
     │   LCD: "Access Denied!" → Box stays locked
     │
     └── No response within 10 seconds?
              ↓
         LCD: "You missed your Medication!"
              ↓
         System resets → Back to current time
```

-----

## 👥 Authors

**Kurt Russell S. Aquino**

- Role: Hardware Engineer & Lead Programmer
- Responsible for circuit design, wiring, and complete Arduino source code

**Salha S. Akmad**

- Role: Technical Writer & Project Documentarian
- Responsible for project proposal, documentation, and hardware specifications write-up

> 2nd Year — BS Computer Engineering
> Notre Dame of Tacurong College

-----

## 📄 License

This project was developed as an academic requirement and is intended for educational purposes only.
