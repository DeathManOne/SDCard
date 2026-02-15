# SDCard
[![Ask DeepWiki](https://devin.ai/assets/askdeepwiki.png)](https://deepwiki.com/DeathManOne/SDCard)

A simple C++ library for reading from and writing to an SD card using SPI on an ESP32 with the Arduino framework.

This library provides a clean, object-oriented interface for common file and directory operations on an SD card.

## Features

*   **File Operations**: Read, write, append, rename, erase, and delete files.
*   **Directory Management**: Create, remove, and check for the existence of directories.
*   **Recursive Directory Listing**: List all files in a directory and its subdirectories up to a specified depth.
*   **Simplified Pathing**: Automatically handles absolute paths, so you don't need to worry about leading slashes.

## Installation

You can install this library through the Arduino Library Manager:

1.  Open the Arduino IDE.
2.  Go to `Sketch` -> `Include Library` -> `Manage Libraries...`.
3.  Search for "SDCard" by DeathManOne.
4.  Click the "Install" button.

Alternatively, you can clone this repository into your Arduino `libraries` folder.

## Usage Example

Here's a basic example of how to initialize the SD card, write to a file, read it back, list all files, and then delete the file.

Make sure to connect your SD card module to the ESP32's default VSPI pins:

*   **CS**: GPIO 5
*   **SCK**: GPIO 18
*   **MISO**: GPIO 19
*   **MOSI**: GPIO 23

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <SDCard.h>

// Define CS pin for SD card module
const int SD_CS_PIN = 5;

// Create an instance of the SPIClass for VSPI
SPIClass spi(VSPI);

// Create an instance of the SDCard library
SDCard sdCard;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect

  // Initialize SPI bus
  spi.begin();

  // Initialize the SD card
  Serial.println("Initializing SD card...");
  if (!sdCard.initialize(SD_CS_PIN, spi)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done.");

  // --- File Operations ---
  std::string filename = "hello.txt";
  std::string message = "Hello from ESP32!";

  // Write to a file
  Serial.println("Writing to file: " + String(filename.c_str()));
  if (sdCard.fileWriteOrAppend(filename, message)) {
    Serial.println("Write successful.");
  } else {
    Serial.println("Write failed.");
  }

  // Read from the file
  std::string content = "";
  Serial.println("Reading from file: " + String(filename.c_str()));
  if (sdCard.fileRead(filename, content)) {
    Serial.println("Read successful. Content:");
    Serial.println(content.c_str());
  } else {
    Serial.println("Read failed.");
  }

  // --- Directory Listing ---
  Serial.println("Listing files on SD card:");
  std::map<std::string, size_t> fileList = sdCard.dirList("/", 1);
  if (fileList.empty()) {
      Serial.println("No files found or directory is empty.");
  } else {
      for (auto const& [path, size] : fileList) {
          Serial.printf("File: %s, Size: %d bytes\n", path.c_str(), size);
      }
  }
  
  // --- Delete File ---
  Serial.println("Deleting file: " + String(filename.c_str()));
  if(sdCard.fileDelete(filename)) {
    Serial.println("File deleted successfully.");
  } else {
    Serial.println("Failed to delete file.");
  }

  // Verify deletion
  if (!sdCard.dirOrFileExist(filename)) {
    Serial.println("File no longer exists.");
  }
}

void loop() {
  // Nothing to do here
}
```

## API Reference

### `SDCard()`
Constructor for the `SDCard` class.

---
### `bool initialize(int SDA, SPIClass &SPI) const`
Initializes the library and the SD card. SPI must be initialized before calling this method.
*   **`SDA`**: The Chip Select (CS) pin for the SD card module.
*   **`SPI`**: An initialized `SPIClass` object (e.g., `SPI` or `VSPI`).
*   **Returns**: `true` if initialization is successful, `false` otherwise.

---
### `bool dirOrFileExist(std::string dirnameOrFilename) const`
Checks whether a file or directory exists.
*   **`dirnameOrFilename`**: The absolute path of the file or directory.
*   **Returns**: `true` if it exists, `false` otherwise.

---
### `std::map<std::string, size_t> dirList(std::string dirname = "/", int maxLevel = 0) const`
Gets a list of files and their sizes within a directory.
*   **`dirname`**: The absolute path of the directory to list (default: `/`).
*   **`maxLevel`**: The depth of sub-directories to traverse (default: `0`, meaning only the given directory).
*   **Returns**: A `std::map` where the key is the absolute file path and the value is the file size.

---
### `bool dirCreate(std::string dirname) const`
Creates a new directory.
*   **`dirname`**: The absolute path of the new directory.
*   **Returns**: `true` if the directory was created or already exists, `false` on failure.

---
### `bool dirRemove(std::string dirname) const`
Removes an existing, empty directory.
*   **`dirname`**: The absolute path of the directory to remove.
*   **Returns**: `true` if the directory was removed or does not exist, `false` on failure.

---
### `bool fileRead(std::string filename, std::string &result) const`
Reads the entire content of a file.
*   **`filename`**: The absolute path of the file to read.
*   **`result`**: A `std::string` reference where the file content will be stored.
*   **Returns**: `true` if the read was successful, `false` otherwise.

---
### `bool fileWriteOrAppend(std::string filename, std::string message) const`
Writes content to a file. If the file exists, it appends the content. If not, it creates the file and writes the content. A newline character is automatically added.
*   **`filename`**: The absolute path of the file.
*   **`message`**: The `std::string` content to write.
*   **Returns**: `true` if the write/append was successful, `false` otherwise.

---
### `bool fileErase(std::string filename) const`
Clears all data in a file by overwriting it, without deleting the file itself.
*   **`filename`**: The absolute path of the existing file to erase.
*   **Returns**: `true` if the content was erased, `false` otherwise.

---
### `bool fileRename(std::string fromFilename, std::string toFilename) const`
Renames or moves a file.
*   **`fromFilename`**: The original absolute path of the file.
*   **`toFilename`**: The new absolute path for the file.
*   **Returns**: `true` if the file was renamed successfully, `false` otherwise.

---
### `bool fileDelete(std::string filename) const`
Deletes a file.
*   **`filename`**: The absolute path of the file to delete.
*   **Returns**: `true` if the file was deleted or did not exist, `false` on failure.

## License
This project is released under the [Unlicense](https://unlicense.org/), which is a public domain equivalent license.
