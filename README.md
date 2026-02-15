
# SDCard for ESP32
## Initialization
```c++
#include <SDcard.h>
SDCard *_SD_CARD;
int _SD_CARD_PIN = 7;

void setup() {
  SPI.begin();
  _SD_CARD = new SDCard();

  bool sdEnable = false;
  do { sdEnable = _SD_CARD->initialize(_SD_CARD_PIN, SPI); }
  while (!sdEnable);
}
```

## Delete
```c++
delete _SD_CARD;
```

## Commands
### -> Both files and directories <-
#### Check existing file or directory
```c++
std::string toCheck= "/path/to/myFile.txt";
//std::string toCheck = "/myDirectory";

if (_SD_CARD->dirOrFileExist(toCheck))
  { Serial.println("File or directory exist"); }
else { Serial.println("File or directory does not exist"); }
```

### -> Directories <-
#### Create new directory
```c++
std::string dirname = "/path/to/myNewDirectory";

if (_SD_CARD->dirCreate(dirname))
  { Serial.println("Directory created"); }
else { Serial.println("Directory not created"); }
```

#### Remove directory
```c++
std::string dirname = "/path/to/directoryToDelete";

if (_SD_CARD->dirRemove(dirname))
  { Serial.println("Directory deleted"); }
else { Serial.println("Directory not deleted"); }
```

### -> Files <-
#### List of files
###### If a directory is empty (no file), it will not be seen
```c++
std::string myDir = "/"; // root
int maxLevel = 4; // four sub-directories max

std::map<std::string, size_t> files = SD_CARD->dirList(myDir, maxLevel);
for (auto file : files)
  { Serial.printf("%s (%d)\n", file.first.c_str(), file.second); }
```

#### Read file
```c++
std::string filename = "/path/to/myFile.txt";
std::string result;

if (_SD_CARD->read(filename, result))
  { Serial.printf("File: %s\nData:\n%s\n", filename.c_str(), result.c_str()); }
else { Serial.println("File not read"); }
```

#### Create new file and write / append in existing file
```c++
std::string filename = "/path/to/myFile.txt";
std::string data = "Hello world, from ESP32";

if (_SD_CARD->fileWriteOrAppend(filename, data))
  { Serial.println("Written"); }
else { Serial.println("Not written"); }
```

#### Erase all datas in file
```c++
std::string filename = "/path/to/myFile.txt";

if (_SD_CARD->fileErase(filename))
  { Serial.println("File erased"); }
else { Serial.println("File not erased"); }
```

#### Rename file
```c++
std::string fromFilename= "/path/to/myOldFile.txt";
std::string toFilename= "/path/to/myNewFile.txt";

if (_SD_CARD->fileRename(fromFilename, toFilename))
  { Serial.println("File renamed"); }
else { Serial.println("File not renamed"); }
```

#### Delete file
```c++
std::string filename= "/path/to/fileToDelete.txt";

if (_SD_CARD->fileDelete(filename))
  { Serial.println("File deleted"); }
else { Serial.println("File not deleted"); }
```
