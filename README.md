# SDCard for ESP32

## Complete Documentation (v2.0.0)

This library provides complete SD card management for ESP32 using SPI with a focus on:
- Zero STL in public API
- No dynamic memory allocation
- Fixed-size buffers
- Callback-based reading
- Automatic parent directory creation

---

# Table of Contents

1. Introduction
2. Features
3. Installation
4. Quick Start
5. SD Card Management
6. Directory Management
7. File Information
8. File Reading
9. Text Writing
10. Binary Writing
11. File Management
12. Callbacks
13. Constants
14. Path Normalization
15. Return Values
16. Migration from 1.x
17. Complete Example
18. License

---

# Introduction

SDCard is a lightweight SD card library for ESP32 using SPI.

Unlike many filesystem wrappers, version 2.0.0 removes STL containers from the public API and relies on callbacks and fixed buffers to provide predictable memory usage suitable for embedded systems.

---

# Features

- SD card initialization
- Card information retrieval
- Recursive directory listing
- Recursive directory deletion
- Text file reading and writing
- Binary file reading and writing
- File copy, rename and delete
- Automatic parent directory creation
- Callback-based streaming
- Fixed-size buffers
- No heap allocations in normal operation

---

# Installation

## PlatformIO

```ini
lib_deps =
    https://github.com/DeathManOne/SDCard.git
```

## Arduino IDE

Install the library inside the Arduino libraries directory.

---

# Quick Start

```cpp
#include <SPI.h>
#include <SDCard.h>

SDCard sd;

void setup() {
    SPI.begin();

    if (!sd.initialize(SPI, 5)) {
        return;
    }

    sd.fileWrite("/hello.txt", "Hello World");
}
```

---

# SD Card Management

## initialize()

Initializes the SD card.

```cpp
bool initialize(SPIClass &spi, int cs);
```

## isInitialized()

```cpp
bool isInitialized() const;
```

## cardInfos()

```cpp
bool cardInfos(
    uint8_t &type,
    uint64_t &size,
    uint64_t &totalBytes,
    uint64_t &usedBytes
);
```

---

# Directory Management

## dirExists()

```cpp
bool dirExists(const char *dirname);
```

## dirCreate()

Creates a directory and missing parents.

```cpp
bool dirCreate(const char *dirname);
```

## dirRemove()

Removes an empty directory.

```cpp
bool dirRemove(const char *dirname);
```

## dirRemoveRecursive()

Removes a directory and all contents.

```cpp
bool dirRemoveRecursive(const char *dirname);
```

## dirList()

```cpp
bool dirList(
    const char *dirname,
    int maxLevel,
    bool includeDirectories,
    DirListCallback callback,
    void *userData = nullptr
);
```

Example:

```cpp
bool callback(
    const char *path,
    size_t size,
    bool isDirectory,
    void *userData
) {
    return true;
}

sd.dirList("/", 4, true, callback);
```

---

# File Information

## fileExists()

```cpp
bool fileExists(const char *filename);
```

## fileSize()

```cpp
size_t fileSize(const char *filename);
```

---

# File Reading

## fileRead()

Reads a file by chunks.

```cpp
bool fileRead(
    const char *filename,
    FileReadCallback callback,
    void *userData = nullptr
);
```

Example:

```cpp
bool callback(
    const uint8_t *buffer,
    size_t length,
    void *userData
) {
    Serial.write(buffer, length);
    return true;
}
```

Returning false stops reading.

## fileReadLines()

```cpp
bool fileReadLines(
    const char *filename,
    FileReadLineCallback callback,
    void *userData = nullptr
);
```

Example:

```cpp
bool callback(
    const char *line,
    void *userData
) {
    Serial.println(line);
    return true;
}
```

---

# Text Writing

## fileWrite()

```cpp
bool fileWrite(
    const char *filename,
    const char *message,
    bool addNewLine = false
);
```

## fileAppend()

```cpp
bool fileAppend(
    const char *filename,
    const char *message,
    bool addNewLine = false
);
```

## fileWriteOrAppend()

```cpp
bool fileWriteOrAppend(
    const char *filename,
    const char *message,
    bool addNewLine = false
);
```

---

# Binary Writing

## fileWrite()

```cpp
bool fileWrite(
    const char *filename,
    const uint8_t *buffer,
    size_t length
);
```

## fileAppend()

```cpp
bool fileAppend(
    const char *filename,
    const uint8_t *buffer,
    size_t length
);
```

## fileWriteOrAppend()

```cpp
bool fileWriteOrAppend(
    const char *filename,
    const uint8_t *buffer,
    size_t length
);
```

These methods are ideal for binary data and embedded null bytes.

---

# File Management

## fileCopy()

```cpp
bool fileCopy(
    const char *fromFilename,
    const char *toFilename
);
```

## fileRename()

```cpp
bool fileRename(
    const char *fromFilename,
    const char *toFilename
);
```

## fileDelete()

```cpp
bool fileDelete(const char *filename);
```

## fileErase()

```cpp
bool fileErase(const char *filename);
```

---

# Callbacks

## FileReadCallback

```cpp
bool callback(
    const uint8_t *buffer,
    size_t length,
    void *userData
);
```

## FileReadLineCallback

```cpp
bool callback(
    const char *line,
    void *userData
);
```

## DirListCallback

```cpp
bool callback(
    const char *path,
    size_t size,
    bool isDirectory,
    void *userData
);
```

Returning false stops the current operation.

---

# Constants

```cpp
SDCard::MAX_RECURSIVE_DEPTH
SDCard::MAX_PATH_LENGTH
SDCard::MAX_LINE_LENGTH
SDCard::FILE_BUFFER_SIZE
```

Default values:

- MAX_RECURSIVE_DEPTH = 16
- MAX_PATH_LENGTH = 256
- MAX_LINE_LENGTH = 512
- FILE_BUFFER_SIZE = 512

---

# Path Normalization

Examples:

logs/data.txt

becomes

/logs/data.txt

and

//logs///data.txt

becomes

/logs/data.txt

---

# Return Values

Unless otherwise specified:

- true = success
- false = failure

---

# Migration from 1.x

Version 2.0.0 removes:

- std::string
- std::vector
- std::map
- std::function

and replaces them with:

- const char *
- callbacks
- userData
- fixed-size buffers

---

# Complete Example

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <SDCard.h>

SDCard sd;

bool lineCallback(const char *line, void *userData) {
    Serial.println(line);
    return true;
}

void setup() {
    Serial.begin(115200);

    SPI.begin();

    if (!sd.initialize(SPI, 5))
        return;

    sd.fileWrite("/logs/boot.txt", "ESP32 started", true);

    sd.fileReadLines("/logs/boot.txt", lineCallback);
}

void loop() {
}
```

---

# License

GPL v3 or later.
