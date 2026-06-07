# UI-Based-Digital-Clock
A UI-based Digital Clock Application built in C++ using SFML, featuring a Live Clock, Alarm, Stopwatch, Countdown Timer, and Calendar Display. Developed for Programming Fundamentals (BS-DS) at FAST Islamabad.

# Lab Report: Digital Clock Application
**Course:** Programming Fundamentals
**Program:** BS(DS)

---

## 1. Objective

The objective of this lab was to design and implement a fully functional Digital Clock Application using C++ and SFML graphics library. The application demonstrates the use of fundamental programming concepts including arrays, pointers, functions, loops, conditionals, and file handling — all without the use of classes, structs, vectors, or STL containers.

---

## 2. Tools and Technologies Used

- **Language:** C++ (C++11 standard)
- **Graphics Library:** SFML (Simple and Fast Multimedia Library)
- **Compiler:** GCC / G++
- **IDE:** Any text editor or IDE (e.g., VS Code, Code::Blocks)
- **OS Compatibility:** Windows, Linux, macOS

---

## 3. Application Features

### 3.1 Live Digital Clock
The application displays a real-time digital clock that updates every second. The time is fetched from the system using the `time()` and `localtime()` functions from `<ctime>`. The user can toggle between 12-hour and 24-hour formats by pressing the Space key. The current date including month, day, and year is also displayed.

### 3.2 Alarm
The alarm feature allows the user to set an alarm by entering a time in HHMM format. When the system time matches the alarm time, a flashing red alert is displayed on the screen. The alarm can be cleared at any time. Alarm settings are automatically saved to a file and reloaded on the next program launch, ensuring persistence across sessions.

### 3.3 Stopwatch
The stopwatch feature supports start, pause, resume, and reset operations. It tracks hours, minutes, seconds, and centiseconds with precision using delta time calculations from SFML's `sf::Clock`. The user can also record lap times which are saved to an external file with timestamps.

### 3.4 Countdown Timer
The countdown timer allows the user to input a custom time in HHMMSS format and count down to zero. When the timer reaches zero, a flashing orange alert notifies the user. The timer supports start, pause, and reset functionality.

### 3.5 Menu-Based Navigation
A main menu screen provides numbered options for each feature. The user navigates using keyboard number keys. Inside each feature, the ESC key returns to the main menu, keeping the interface clean and intuitive.

---

## 4. Programming Concepts Applied

### 4.1 Primitive Data Types and Variables
Global integer variables were used to store time values, alarm settings, stopwatch counters, and timer counters. Boolean flags were used to track states such as whether the alarm is set, whether the stopwatch is running, and so on.

### 4.2 Functions
The application is organized into multiple well-defined functions, each responsible for a specific task such as `updateTime()`, `updateStopwatch()`, `updateTimer()`, `checkAlarm()`, `drawMenu()`, `drawClock()`, and so on. This promotes modularity and readability.

### 4.3 Pass-by-Value and Pass-by-Reference
The `convertTo12Hour()` function demonstrates pass-by-reference using pointers, taking a `int*` for the display hour and a `bool*` for the AM/PM flag and modifying them directly.

### 4.4 Arrays and Char Arrays
Character arrays (`char[]`) were used throughout the application as string buffers for building and displaying time strings, date strings, and user input. A char array `inputBuffer[10]` stores user keyboard input for alarm and timer setting.

### 4.5 Pointers
Pointers were used in the `convertTo12Hour()` function for pass-by-reference behavior and during file parsing in `loadAlarmFromFile()` and `loadPreferences()` where a `char*` pointer traverses the buffer to extract values.

### 4.6 Loops and Conditionals
For loops were used to iterate over font path arrays and menu options. While loops were used inside `intToString()` for digit extraction. If/else chains and switch-like structures were used throughout for event handling and state management.

### 4.7 File Handling
Three files are used for persistent storage:
- `alarm_settings.txt` — stores alarm hour, minute, and enabled status
- `user_preferences.txt` — stores the selected time format (12 or 24 hour)
- `stopwatch_laps.txt` — appends lap times with timestamps

`ofstream` is used for writing, `ifstream` for reading, and `ios::app` flag for appending lap records without overwriting previous ones.

### 4.8 SFML Graphics
SFML was used to build the entire graphical user interface. `sf::RenderWindow` manages the application window. `sf::Text` and `sf::Font` handle all text rendering. `sf::RectangleShape` draws buttons and alert banners. `sf::Clock` provides precise time measurement for stopwatch and timer delta calculations. Flashing effects were achieved using `std::sin()` on clock ticks to produce smooth alpha oscillation.

---

## 5. Application Flow

```
Program Start
     │
     ▼
Load Files (alarm_settings.txt, user_preferences.txt)
     │
     ▼
Main Menu
  ├── 1. Live Clock  → Toggle 12/24 format, display date & time
  ├── 2. Alarm       → Set/clear alarm, notify on trigger
  ├── 3. Stopwatch   → Start/pause/reset, save laps to file
  ├── 4. Timer       → Set time, countdown, notify on finish
  └── 5. Exit        → Close application
```

---

## 6. Challenges Faced

- **Font Loading:** SFML requires an external `.ttf` font file. Multiple system font paths were hardcoded as a fallback array to maximize compatibility across operating systems.
- **Time Precision:** The stopwatch required delta time from SFML's `sf::Clock` rather than system time to achieve centisecond-level precision.
- **File Parsing Without STL:** Since `std::string` and STL were avoided in parsing, a `char*` pointer was used to manually traverse lines and extract integer values from the settings files.
- **Input Handling:** Keyboard input for alarm and timer required careful buffer management using a char array and index tracking without any string library functions.

---

## 7. Conclusion

This lab successfully demonstrates the implementation of a fully functional Digital Clock Application using fundamental C++ programming constructs. All required features — live clock, alarm, stopwatch, countdown timer, and menu navigation — were implemented without the use of classes, structs, vectors, or STL. File handling was used to achieve persistent storage of alarm settings, user preferences, and lap records. The SFML library was used to create an interactive and visually clear graphical interface. This project reinforced the practical application of pointers, arrays, functions, loops, conditionals, and file I/O in a real-world context.