#include "../include/SDCard.h"

SDCard::SDCard() {
    this->_SD = new fs::SDFS(SD);
}

SDCard::~SDCard() {
    this->_SD->end();
    delete this->_SD;
}

bool SDCard::initialize(int SDA, SPIClass &SPI) const {
    return this->_SD->begin(SDA, SPI);
}

std::map<std::string, size_t> SDCard::dirList(std::string dirname, int maxLevel) const {
    if (dirname.c_str()[0] != '/')
        { dirname = "/" + dirname; }

    std::map<std::string, size_t> tmp = {};
    File dirOrFile = this->_SD->open(dirname.c_str());

    if (!dirOrFile) { return tmp; }
    if (!dirOrFile.isDirectory()) { return tmp; }

    File file = dirOrFile.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            if (maxLevel <= 0) {
                file = dirOrFile.openNextFile();
                continue;
            }
            std::map<std::string, size_t> subDir =
                this->dirList(dirname + "/" + file.name(), maxLevel - 1);
            for (auto dir : subDir)
                { tmp.insert({dir.first, dir.second}); }
        } else { tmp.insert({dirname + "/" + file.name(), file.size()}); }
        file = dirOrFile.openNextFile();
    }
    return tmp;
}

bool SDCard::dirOrFileExist(std::string dirnameOrFilename) const {
    if (dirnameOrFilename.c_str()[0] != '/')
        { dirnameOrFilename = "/" + dirnameOrFilename; }

    return this->_SD->exists(dirnameOrFilename.c_str());
}

bool SDCard::dirCreate(std::string dirname) const {
    if (dirname.c_str()[0] != '/')
        { dirname = "/" + dirname; }
    if (this->_SD->exists(dirname.c_str()))
        { return true; }

    return this->_SD->mkdir(dirname.c_str());
}

bool SDCard::dirRemove(std::string dirname) const {
    if (dirname.c_str()[0] != '/')
        { dirname = "/" + dirname; }
    if (!this->_SD->exists(dirname.c_str()))
        { return true; }

    return this->_SD->rmdir(dirname.c_str());
}

bool SDCard::fileRead(std::string filename, std::string &result) const {
    if (filename.c_str()[0] != '/')
        { filename = "/" + filename; }
    if (!this->_SD->exists(filename.c_str()))
        { return false; }

    File file = this->_SD->open(filename.c_str(), FILE_READ, false);
    if (!file)
        { return false; }

    result = "";
    while (file.available())
        { result.append(file.readString().c_str()); }

    file.close();
    return true;
}

bool SDCard::fileWriteOrAppend(std::string filename, std::string message) const {
    if (filename.c_str()[0] != '/')
        { filename = "/" + filename; }

    File file;
    if (this->_SD->exists(filename.c_str()))
        { file = this->_SD->open(filename.c_str(), FILE_APPEND, false); }
    else { file = this->_SD->open(filename.c_str(), FILE_WRITE, true); }

    if (!file)
        { return false; }
    size_t written = file.print((message.append("\n")).c_str());
    file.close();

    return written != 0;
}

bool SDCard::fileErase(std::string filename) const {
    if (filename.c_str()[0] != '/')
        { filename = "/" + filename; }
    if (!this->_SD->exists(filename.c_str()))
        { return false; }

    File file = this->_SD->open(filename.c_str(), FILE_WRITE, false);
    if (!file)
        { return false; }
    size_t written = file.print(" ");

    file.close();
    return written != 0;
}

bool SDCard::fileRename(std::string fromFilename, std::string toFilename) const {
    if (fromFilename.c_str()[0] != '/')
        { fromFilename = "/" + fromFilename; }
    if (toFilename.c_str()[0] != '/')
        { toFilename = "/" + toFilename; }

    if (!this->_SD->exists(fromFilename.c_str()))
        { return false; }
    if (this->_SD->exists(toFilename.c_str()))
        { return false; }

    return this->_SD->rename(fromFilename.c_str(), toFilename.c_str());
}

bool SDCard::fileDelete(std::string filename) const {
    if (filename.c_str()[0] != '/')
        { filename = "/" + filename; }
    if (!this->_SD->exists(filename.c_str()))
        { return true; }
    return this->_SD->remove(filename.c_str());
}

