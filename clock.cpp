//g++ -std=c++11 clock.cpp -o clock -lsfml-graphics -lsfml-window -lsfml-system
//./clock
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <ctime>
#include <cstring>
#include <cmath>
#include <fstream>
#include <sstream>

// Global variables for time tracking
int currentHour = 0, currentMinute = 0, currentSecond = 0;
bool is24HourFormat = true;

// Alarm variables
int alarmHour = -1, alarmMinute = -1;
bool alarmSet = false, alarmTriggered = false;

// Stopwatch variables
int swHours = 0, swMinutes = 0, swSeconds = 0, swMilliseconds = 0;
bool swRunning = false;

// Timer variables
int timerHours = 0, timerMinutes = 0, timerSeconds = 0, timerMilliseconds = 0;
bool timerRunning = false, timerFinished = false;

// Menu state
int currentMenu = 0; // 0=main, 1=clock, 2=alarm, 3=stopwatch, 4=timer
int inputMode = 0; // For alarm/timer input

// Input buffers
char inputBuffer[10];
int inputIndex = 0;

// Function prototypes
void updateTime();
void checkAlarm();
void updateStopwatch(float dt);
void updateTimer(float dt);
void convertTo12Hour(int hour, int* displayHour, bool* isPM);
void intToString(int num, char* buffer, int minDigits);
void drawClock(sf::RenderWindow& window, sf::Font& font);
void drawAlarm(sf::RenderWindow& window, sf::Font& font);
void drawStopwatch(sf::RenderWindow& window, sf::Font& font);
void drawTimer(sf::RenderWindow& window, sf::Font& font);
void drawMenu(sf::RenderWindow& window, sf::Font& font);
void handleClockInput(sf::Event& event);
void handleAlarmInput(sf::Event& event);
void handleStopwatchInput(sf::Event& event);
void handleTimerInput(sf::Event& event);
void saveAlarmToFile();
void loadAlarmFromFile();
void drawButton(sf::RenderWindow& window, sf::Font& font, const char* text, float x, float y, float w, float h, bool highlight);

// Update system time
void updateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    currentHour = ltm->tm_hour;
    currentMinute = ltm->tm_min;
    currentSecond = ltm->tm_sec;
}

// Check if alarm should trigger
void checkAlarm() {
    if (alarmSet && !alarmTriggered) {
        if (currentHour == alarmHour && currentMinute == alarmMinute) {
            alarmTriggered = true;
        }
    }
}

// Update stopwatch
void updateStopwatch(float dt) {
    if (swRunning) {
        swMilliseconds += (int)(dt * 1000);
        if (swMilliseconds >= 1000) {
            swSeconds += swMilliseconds / 1000;
            swMilliseconds %= 1000;
        }
        if (swSeconds >= 60) {
            swMinutes += swSeconds / 60;
            swSeconds %= 60;
        }
        if (swMinutes >= 60) {
            swHours += swMinutes / 60;
            swMinutes %= 60;
        }
    }
}

// Update countdown timer
void updateTimer(float dt) {
    if (timerRunning) {
        timerMilliseconds -= (int)(dt * 1000);
        if (timerMilliseconds < 0) {
            timerSeconds--;
            timerMilliseconds += 1000;
        }
        if (timerSeconds < 0 && (timerMinutes > 0 || timerHours > 0)) {
            timerMinutes--;
            timerSeconds += 60;
        }
        if (timerMinutes < 0 && timerHours > 0) {
            timerHours--;
            timerMinutes += 60;
        }
        if (timerHours <= 0 && timerMinutes <= 0 && timerSeconds <= 0) {
            timerHours = 0; timerMinutes = 0; timerSeconds = 0; timerMilliseconds = 0;
            timerRunning = false;
            timerFinished = true;
        }
    }
}

// Convert 24-hour to 12-hour format
void convertTo12Hour(int hour, int* displayHour, bool* isPM) {
    *isPM = hour >= 12;
    *displayHour = hour % 12;
    if (*displayHour == 0) *displayHour = 12;
}

// Convert integer to string with leading zeros
void intToString(int num, char* buffer, int minDigits) {
    char temp[20];
    int idx = 0;
    if (num == 0) {
        temp[idx++] = '0';
    } else {
        int n = num;
        while (n > 0) {
            temp[idx++] = '0' + (n % 10);
            n /= 10;
        }
    }
    while (idx < minDigits) {
        temp[idx++] = '0';
    }
    for (int i = 0; i < idx; i++) {
        buffer[i] = temp[idx - 1 - i];
    }
    buffer[idx] = '\0';
}

// Draw button helper
void drawButton(sf::RenderWindow& window, sf::Font& font, const char* text, float x, float y, float w, float h, bool highlight) {
    sf::RectangleShape button(sf::Vector2f(w, h));
    button.setPosition(x, y);
    if (highlight) {
        button.setFillColor(sf::Color(70, 130, 180));
    } else {
        button.setFillColor(sf::Color(50, 50, 50));
    }
    button.setOutlineColor(sf::Color(100, 100, 100));
    button.setOutlineThickness(2);
    window.draw(button);
    
    sf::Text btnText;
    btnText.setFont(font);
    btnText.setString(text);
    btnText.setCharacterSize(18);
    sf::FloatRect bounds = btnText.getLocalBounds();
    btnText.setPosition(x + w/2 - bounds.width/2, y + h/2 - 12);
    window.draw(btnText);
}

// Save alarm to file
void saveAlarmToFile() {
    std::ofstream file("alarm.txt");
    if (file.is_open()) {
        file << alarmHour << " " << alarmMinute << " " << alarmSet;
        file.close();
    }
}

// Load alarm from file
void loadAlarmFromFile() {
    std::ifstream file("alarm.txt");
    if (file.is_open()) {
        file >> alarmHour >> alarmMinute >> alarmSet;
        file.close();
    }
}

// Draw clock screen
void drawClock(sf::RenderWindow& window, sf::Font& font) {
    // Title
    sf::Text title;
    title.setFont(font);
    title.setString("DIGITAL CLOCK");
    title.setCharacterSize(32);
    title.setPosition(250, 30);
    window.draw(title);
    
    // Get current date
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    // Date display
    char dateStr[50];
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    std::sprintf(dateStr, "%s %d, %d", months[ltm->tm_mon], ltm->tm_mday, 1900 + ltm->tm_year);
    sf::Text dateText;
    dateText.setFont(font);
    dateText.setString(dateStr);
    dateText.setCharacterSize(24);
    dateText.setPosition(280, 80);
    window.draw(dateText);
    
    // Time display
    char timeStr[20];
    if (is24HourFormat) {
        char h[5], m[5], s[5];
        intToString(currentHour, h, 2);
        intToString(currentMinute, m, 2);
        intToString(currentSecond, s, 2);
        std::sprintf(timeStr, "%s:%s:%s", h, m, s);
    } else {
        int dispHour;
        bool isPM;
        convertTo12Hour(currentHour, &dispHour, &isPM);
        char h[5], m[5], s[5];
        intToString(dispHour, h, 2);
        intToString(currentMinute, m, 2);
        intToString(currentSecond, s, 2);
        std::sprintf(timeStr, "%s:%s:%s %s", h, m, s, isPM ? "PM" : "AM");
    }
    
    sf::Text clockText;
    clockText.setFont(font);
    clockText.setString(timeStr);
    clockText.setCharacterSize(72);
    clockText.setPosition(180, 180);
    clockText.setFillColor(sf::Color::Cyan);
    window.draw(clockText);
    
    // Format toggle button
    drawButton(window, font, is24HourFormat ? "24H Format" : "12H Format", 300, 320, 150, 40, false);
    
    // Instructions
    sf::Text instr;
    instr.setFont(font);
    instr.setString("Press [Space] to toggle format  |  [ESC] for menu");
    instr.setCharacterSize(16);
    instr.setPosition(150, 450);
    instr.setFillColor(sf::Color(150, 150, 150));
    window.draw(instr);
}

// Draw alarm screen
void drawAlarm(sf::RenderWindow& window, sf::Font& font) {
    sf::Text title;
    title.setFont(font);
    title.setString("ALARM CLOCK");
    title.setCharacterSize(32);
    title.setPosition(270, 30);
    window.draw(title);
    
    // Current alarm display
    if (alarmSet) {
        char alarmStr[30];
        char h[5], m[5];
        intToString(alarmHour, h, 2);
        intToString(alarmMinute, m, 2);
        std::sprintf(alarmStr, "Alarm Set: %s:%s", h, m);
        sf::Text alarmText;
        alarmText.setFont(font);
        alarmText.setString(alarmStr);
        alarmText.setCharacterSize(28);
        alarmText.setPosition(250, 120);
        alarmText.setFillColor(sf::Color::Green);
        window.draw(alarmText);
    } else {
        sf::Text noAlarm;
        noAlarm.setFont(font);
        noAlarm.setString("No Alarm Set");
        noAlarm.setCharacterSize(28);
        noAlarm.setPosition(280, 120);
        noAlarm.setFillColor(sf::Color(150, 150, 150));
        window.draw(noAlarm);
    }
    
    // Alarm triggered warning
    if (alarmTriggered) {
        float flashAlpha = 128 + 127 * std::sin(clock() / 200.0);
        sf::RectangleShape flash(sf::Vector2f(700, 100));
        flash.setPosition(50, 200);
        flash.setFillColor(sf::Color(255, 0, 0, (sf::Uint8)flashAlpha));
        window.draw(flash);
        
        sf::Text alert;
        alert.setFont(font);
        alert.setString("!!! ALARM RINGING !!!");
        alert.setCharacterSize(48);
        alert.setPosition(180, 220);
        alert.setFillColor(sf::Color::White);
        window.draw(alert);
    }
    
    // Input mode
    if (inputMode == 1) {
        sf::Text prompt;
        prompt.setFont(font);
        prompt.setString("Enter alarm time (HHMM): ");
        prompt.setCharacterSize(24);
        prompt.setPosition(200, 320);
        window.draw(prompt);
        
        sf::Text input;
        input.setFont(font);
        input.setString(inputBuffer);
        input.setCharacterSize(24);
        input.setPosition(350, 355);
        input.setFillColor(sf::Color::Yellow);
        window.draw(input);
    } else {
        drawButton(window, font, "Set Alarm", 200, 320, 120, 40, false);
        drawButton(window, font, "Clear Alarm", 340, 320, 120, 40, false);
        if (alarmTriggered) {
            drawButton(window, font, "Stop Alarm", 480, 320, 120, 40, false);
        }
    }
    
    sf::Text instr;
    instr.setFont(font);
    instr.setString("Press [S] to set | [C] to clear | [ESC] for menu");
    instr.setCharacterSize(16);
    instr.setPosition(180, 450);
    instr.setFillColor(sf::Color(150, 150, 150));
    window.draw(instr);
}

// Draw stopwatch screen
void drawStopwatch(sf::RenderWindow& window, sf::Font& font) {
    sf::Text title;
    title.setFont(font);
    title.setString("STOPWATCH");
    title.setCharacterSize(32);
    title.setPosition(290, 30);
    window.draw(title);
    
    // Stopwatch display
    char swStr[30];
    char h[5], m[5], s[5], ms[5];
    intToString(swHours, h, 2);
    intToString(swMinutes, m, 2);
    intToString(swSeconds, s, 2);
    intToString(swMilliseconds / 10, ms, 2);
    std::sprintf(swStr, "%s:%s:%s.%s", h, m, s, ms);
    
    sf::Text swText;
    swText.setFont(font);
    swText.setString(swStr);
    swText.setCharacterSize(64);
    swText.setPosition(170, 180);
    swText.setFillColor(sf::Color::Yellow);
    window.draw(swText);
    
    // Control buttons
    drawButton(window, font, swRunning ? "Pause" : "Start", 150, 320, 100, 40, false);
    drawButton(window, font, "Reset", 270, 320, 100, 40, false);
    drawButton(window, font, "Lap", 390, 320, 100, 40, false);
    
    sf::Text instr;
    instr.setFont(font);
    instr.setString("Press [Space] start/pause | [R] reset | [ESC] menu");
    instr.setCharacterSize(16);
    instr.setPosition(160, 450);
    instr.setFillColor(sf::Color(150, 150, 150));
    window.draw(instr);
}

// Draw timer screen
void drawTimer(sf::RenderWindow& window, sf::Font& font) {
    sf::Text title;
    title.setFont(font);
    title.setString("COUNTDOWN TIMER");
    title.setCharacterSize(32);
    title.setPosition(230, 30);
    window.draw(title);
    
    // Timer display
    char tmStr[30];
    char h[5], m[5], s[5];
    intToString(timerHours, h, 2);
    intToString(timerMinutes, m, 2);
    intToString(timerSeconds, s, 2);
    std::sprintf(tmStr, "%s:%s:%s", h, m, s);
    
    sf::Text tmText;
    tmText.setFont(font);
    tmText.setString(tmStr);
    tmText.setCharacterSize(64);
    tmText.setPosition(200, 180);
    if (timerRunning) {
        tmText.setFillColor(sf::Color::Green);
    } else {
        tmText.setFillColor(sf::Color::White);
    }
    window.draw(tmText);
    
    // Timer finished alert
    if (timerFinished) {
        float flashAlpha = 128 + 127 * std::sin(clock() / 150.0);
        sf::RectangleShape flash(sf::Vector2f(700, 80));
        flash.setPosition(50, 120);
        flash.setFillColor(sf::Color(255, 165, 0, (sf::Uint8)flashAlpha));
        window.draw(flash);
        
        sf::Text alert;
        alert.setFont(font);
        alert.setString("TIME'S UP!");
        alert.setCharacterSize(36);
        alert.setPosition(300, 135);
        alert.setFillColor(sf::Color::White);
        window.draw(alert);
    }
    
    // Input or controls
    if (inputMode == 2) {
        sf::Text prompt;
        prompt.setFont(font);
        prompt.setString("Enter time (HHMMSS): ");
        prompt.setCharacterSize(24);
        prompt.setPosition(220, 320);
        window.draw(prompt);
        
        sf::Text input;
        input.setFont(font);
        input.setString(inputBuffer);
        input.setCharacterSize(24);
        input.setPosition(380, 355);
        input.setFillColor(sf::Color::Yellow);
        window.draw(input);
    } else {
        drawButton(window, font, timerRunning ? "Pause" : "Start", 150, 320, 100, 40, false);
        drawButton(window, font, "Set Time", 270, 320, 100, 40, false);
        drawButton(window, font, "Reset", 390, 320, 100, 40, false);
    }
    
    sf::Text instr;
    instr.setFont(font);
    instr.setString("Press [Space] start/pause | [T] set | [R] reset | [ESC] menu");
    instr.setCharacterSize(16);
    instr.setPosition(130, 450);
    instr.setFillColor(sf::Color(150, 150, 150));
    window.draw(instr);
}

// Draw main menu
void drawMenu(sf::RenderWindow& window, sf::Font& font) {
    sf::Text title;
    title.setFont(font);
    title.setString("DIGITAL CLOCK APPLICATION");
    title.setCharacterSize(36);
    title.setPosition(150, 50);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);
    
    // Menu options
    const char* options[] = {"1. Live Clock", "2. Alarm", "3. Stopwatch", "4. Timer", "5. Exit"};
    for (int i = 0; i < 5; i++) {
        sf::Text option;
        option.setFont(font);
        option.setString(options[i]);
        option.setCharacterSize(28);
        option.setPosition(280, 150 + i * 60);
        window.draw(option);
    }
    
    sf::Text instr;
    instr.setFont(font);
    instr.setString("Press number key to select option");
    instr.setCharacterSize(18);
    instr.setPosition(220, 450);
    instr.setFillColor(sf::Color(150, 150, 150));
    window.draw(instr);
}

// Handle clock input
void handleClockInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            is24HourFormat = !is24HourFormat;
        } else if (event.key.code == sf::Keyboard::Escape) {
            currentMenu = 0;
        }
    }
}

// Handle alarm input
void handleAlarmInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (inputMode == 1) {
            if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
                if (inputIndex < 4) {
                    inputBuffer[inputIndex++] = '0' + (event.key.code - sf::Keyboard::Num0);
                    inputBuffer[inputIndex] = '\0';
                }
            } else if (event.key.code == sf::Keyboard::BackSpace && inputIndex > 0) {
                inputBuffer[--inputIndex] = '\0';
            } else if (event.key.code == sf::Keyboard::Enter && inputIndex == 4) {
                int hh = (inputBuffer[0] - '0') * 10 + (inputBuffer[1] - '0');
                int mm = (inputBuffer[2] - '0') * 10 + (inputBuffer[3] - '0');
                if (hh >= 0 && hh < 24 && mm >= 0 && mm < 60) {
                    alarmHour = hh;
                    alarmMinute = mm;
                    alarmSet = true;
                    alarmTriggered = false;
                    saveAlarmToFile();
                }
                inputMode = 0;
                inputIndex = 0;
                inputBuffer[0] = '\0';
            } else if (event.key.code == sf::Keyboard::Escape) {
                inputMode = 0;
                inputIndex = 0;
                inputBuffer[0] = '\0';
            }
        } else {
            if (event.key.code == sf::Keyboard::S) {
                inputMode = 1;
                inputIndex = 0;
                inputBuffer[0] = '\0';
            } else if (event.key.code == sf::Keyboard::C) {
                alarmSet = false;
                alarmTriggered = false;
                saveAlarmToFile();
            } else if (event.key.code == sf::Keyboard::Escape) {
                currentMenu = 0;
            }
        }
    }
}

// Handle stopwatch input
void handleStopwatchInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            swRunning = !swRunning;
        } else if (event.key.code == sf::Keyboard::R) {
            swRunning = false;
            swHours = swMinutes = swSeconds = swMilliseconds = 0;
        } else if (event.key.code == sf::Keyboard::Escape) {
            currentMenu = 0;
        }
    }
}

// Handle timer input
void handleTimerInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (inputMode == 2) {
            if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
                if (inputIndex < 6) {
                    inputBuffer[inputIndex++] = '0' + (event.key.code - sf::Keyboard::Num0);
                    inputBuffer[inputIndex] = '\0';
                }
            } else if (event.key.code == sf::Keyboard::BackSpace && inputIndex > 0) {
                inputBuffer[--inputIndex] = '\0';
            } else if (event.key.code == sf::Keyboard::Enter && inputIndex == 6) {
                int hh = (inputBuffer[0] - '0') * 10 + (inputBuffer[1] - '0');
                int mm = (inputBuffer[2] - '0') * 10 + (inputBuffer[3] - '0');
                int ss = (inputBuffer[4] - '0') * 10 + (inputBuffer[5] - '0');
                if (hh >= 0 && mm >= 0 && mm < 60 && ss >= 0 && ss < 60) {
                    timerHours = hh;
                    timerMinutes = mm;
                    timerSeconds = ss;
                    timerFinished = false;
                }
                inputMode = 0;
                inputIndex = 0;
                inputBuffer[0] = '\0';
            } else if (event.key.code == sf::Keyboard::Escape) {
                inputMode = 0;
                inputIndex = 0;
                inputBuffer[0] = '\0';
            }
        } else {
            if (event.key.code == sf::Keyboard::Space) {
                if (timerHours > 0 || timerMinutes > 0 || timerSeconds > 0) {
                    timerRunning = !timerRunning;
                    timerFinished = false;
                }
            } else if (event.key.code == sf::Keyboard::T) {
                inputMode = 2;
                inputIndex = 0;
                inputBuffer[0] = '\0';
                timerRunning = false;
            } else if (event.key.code == sf::Keyboard::R) {
                timerRunning = false;
                timerHours = timerMinutes = timerSeconds = 0;
                timerFinished = false;
            } else if (event.key.code == sf::Keyboard::Escape) {
                currentMenu = 0;
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 500), "Digital Clock Application");
    window.setFramerateLimit(60);
    
    sf::Font font;
    // Try multiple font paths
    bool fontLoaded = false;
    const char* fontPaths[] = {
        "arial.ttf",
        "Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf"
    };
    
    for (int i = 0; i < 5; i++) {
        if (font.loadFromFile(fontPaths[i])) {
            fontLoaded = true;
            break;
        }
    }
    
    if (!fontLoaded) {
        return -1; // Could not load font
    }
    
    loadAlarmFromFile();
    
    sf::Clock clock;
    sf::Clock updateClock;
    
    inputBuffer[0] = '\0';
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (currentMenu == 0) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) currentMenu = 1;
                    else if (event.key.code == sf::Keyboard::Num2) currentMenu = 2;
                    else if (event.key.code == sf::Keyboard::Num3) currentMenu = 3;
                    else if (event.key.code == sf::Keyboard::Num4) currentMenu = 4;
                    else if (event.key.code == sf::Keyboard::Num5) window.close();
                }
            } else if (currentMenu == 1) {
                handleClockInput(event);
            } else if (currentMenu == 2) {
                handleAlarmInput(event);
            } else if (currentMenu == 3) {
                handleStopwatchInput(event);
            } else if (currentMenu == 4) {
                handleTimerInput(event);
            }
        }
        
        float dt = clock.restart().asSeconds();
        
        // Update time every second
        if (updateClock.getElapsedTime().asSeconds() >= 1.0f) {
            updateTime();
            checkAlarm();
            updateClock.restart();
        }
        
        updateStopwatch(dt);
        updateTimer(dt);
        
        window.clear(sf::Color(20, 20, 30));
        
        if (currentMenu == 0) {
            drawMenu(window, font);
        } else if (currentMenu == 1) {
            drawClock(window, font);
        } else if (currentMenu == 2) {
            drawAlarm(window, font);
        } else if (currentMenu == 3) {
            drawStopwatch(window, font);
        } else if (currentMenu == 4) {
            drawTimer(window, font);
        }
        
        window.display();
    }
    
    return 0;
}