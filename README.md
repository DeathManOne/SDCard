# SDCard for ESP32
Simple and complete SD card management library for ESP32 using SPI.
Compatible with Arduino framework and PlatformIO.

---
## Features
* Initialize SD card with SPI
* Read and write files
* Append data in existing files
* Automatically create parent directories
* Copy, rename and delete files
* Read files line by line
* List directories recursively
* Create and remove directories
* Remove directories recursively
* Retrieve SD card informations
* Safe checks when SD card is not initialized

---
# Installation
## PlatformIO
Add in `platformio.ini`:
```ini
lib_deps =
    https://github.com/DeathManOne/SDCard.git
```
## Arduino IDE
Clone or download the repository into your `libraries` folder.

---
# Initialization
```cpp
#include <SPI.h>
#include <SDCard.h>

SDCard *_SD_CARD;

void setup() {
    SPI.begin();

    _SD_CARD = new SDCard();

    bool sdEnable = false;
    do {
        sdEnable = _SD_CARD->initialize(SPI, 5);
    } while (!sdEnable);
}
```
## Delete object
```cpp
delete _SD_CARD;
```

---
# Methods

---
# SD card informations
## Check initialization
```cpp
if (_SD_CARD->isInitialized())
    { Serial.println("SD card initialized"); }
else { Serial.println("SD card not initialized"); }
```
## Get SD card informations
```cpp
uint8_t type;
uint64_t size;
uint64_t totalBytes;
uint64_t usedBytes;

if (_SD_CARD->cardInfos(type, size, totalBytes, usedBytes)) {
    Serial.printf("Type: %u\n", type);
    Serial.printf("Size: %llu MB\n", size / 1024 / 1024);
    Serial.printf("Total bytes: %llu\n", totalBytes);
    Serial.printf("Used bytes: %llu\n", usedBytes);
}
```

---
# Directories
## List files and directories
```cpp
std::map<std::string, size_t> files =
    _SD_CARD->dirList("/", 4, true);

for (auto file : files)
    { Serial.printf("%s (%u bytes)\n", file.first.c_str(), file.second); }
```
### Parameters
| Parameter            | Description                         |
| -------------------- | ----------------------------------- |
| `dirname`            | Directory to explore                |
| `maxLevel`           | Maximum recursive depth             |
| `includeDirectories` | Include directories in returned map |

---
## Check if directory exists
```cpp
if (_SD_CARD->dirExists("/logs"))
    { Serial.println("Directory exists"); }
```

---
## Create directory
Parent directories are automatically created.
```cpp
if (_SD_CARD->dirCreate("/logs/2026/may"))
    { Serial.println("Directory created"); }
```

---
## Remove empty directory
```cpp
if (_SD_CARD->dirRemove("/logs/old"))
    { Serial.println("Directory removed"); }
```

---
## Remove directory recursively
```cpp
if (_SD_CARD->dirRemoveRecursive("/logs"))
    { Serial.println("Directory removed recursively"); }
```

---
# Files
## Check if file exists
```cpp
if (_SD_CARD->fileExists("/logs/data.txt"))
    { Serial.println("File exists"); }
```

---
## Get file size
```cpp
size_t size = _SD_CARD->fileSize("/logs/data.txt");

Serial.printf("File size: %u bytes\n", size);
```

---
## Read file
```cpp
std::string result;

if (_SD_CARD->fileRead("/logs/data.txt", result))
    { Serial.printf("%s\n", result.c_str()); }
```

---
## Read file line by line
```cpp
std::vector<std::string> lines;

if (_SD_CARD->fileReadLines("/logs/data.txt", lines)) {
    for (const std::string &line : lines)
        { Serial.println(line.c_str()); }
}
```

---
## Write file
Create or overwrite a file.
```cpp
if (_SD_CARD->fileWrite("/logs/data.txt", "Hello world"))
    { Serial.println("File written"); }
```
Parent directories are automatically created.

---
## Append in existing file
```cpp
if (_SD_CARD->fileAppend("/logs/data.txt", "New line"))
    { Serial.println("Data appended"); }
```

---
## Write or append
* Create file if it does not exist
* Append data if file already exists
```cpp
if (_SD_CARD->fileWriteOrAppend("/logs/data.txt", "Hello"))
    { Serial.println("Written or appended"); }
```

---
## Copy file
```cpp
if (_SD_CARD->fileCopy("/logs/data.txt", "/backup/data.txt"))
    { Serial.println("File copied"); }
```

Parent directories are automatically created.

---
## Erase file content
File stays existing but becomes empty.
```cpp
if (_SD_CARD->fileErase("/logs/data.txt"))
    { Serial.println("File erased"); }
```

---
## Rename or move file
```cpp
if (_SD_CARD->fileRename("/logs/data.txt", "/backup/data.txt"))
    { Serial.println("File renamed or moved"); }
```
Parent directories are automatically created.

---
## Delete file
```cpp
if (_SD_CARD->fileDelete("/logs/data.txt"))
    { Serial.println("File deleted"); }
```

---
# Notes
* All paths are automatically normalized.
* Missing leading `/` is automatically added.
* Most methods safely return `false` if SD card is not initialized.
* Written messages automatically add a newline (`\n`).

---
# Example
```cpp
#include <Arduino.h>
#include <SPI.h>
#include <SDCard.h>

SDCard sd;

void setup() {
    Serial.begin(115200);

    SPI.begin();

    if (!sd.initialize(SPI, 5)) {
        Serial.println("SD initialization failed");
        return;
    }

    sd.fileWriteOrAppend("/logs/boot.txt", "ESP32 started");

    std::string result;
    if (sd.fileRead("/logs/boot.txt", result))
        { Serial.println(result.c_str()); }
}

void loop() {
}
```

---
# License
This project is licensed under the GNU GPL v3 or later.
See:
```text
LICENSE
```
for full license information.
