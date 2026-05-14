/*
 * files.cpp
 *
 * Copyright (c) 2026 DeathManOne
 * https://github.com/DeathManOne
 * 
 * This file is part of the SDCard library.
 *
 * SDCard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SDCard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SDCard.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "../include/SDCard.h"

bool SDCard::fileExists(std::string filename) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);

    File file = this->_SD->open(filename.c_str());
    if (!file)
        { return false; }
    bool result = !file.isDirectory();
    file.close();
    return result;
}

size_t SDCard::fileSize(std::string filename) {
    if (!this->isInitialized())
        { return 0; }
    filename = this->_normalizePath(filename);

    File file = this->_SD->open(filename.c_str(), FILE_READ, false);
    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return 0;
    }
    size_t result = file.size();
    file.close();
    return result;
}

bool SDCard::fileRead(std::string filename, std::string &result) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);

    if (!this->_SD->exists(filename.c_str()))
        { return false; }
    File file = this->_SD->open(filename.c_str(), FILE_READ, false);

    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }
    result.clear();

    char buffer[512];
    while (file.available()) {
        size_t len = file.readBytes(buffer, sizeof(buffer));
        result.append(buffer, len);
    }
    file.close();
    return true;
}

bool SDCard::fileReadLines(std::string filename, std::vector<std::string> &lines) {
    if (!this->isInitialized())
        { return false; }
    std::string content;

    if (!this->fileRead(filename, content))
        { return false; }

    lines.clear();
    std::string line;
    for (char c : content) {
        if (c == '\n') {
            if (!line.empty() && line.back() == '\r')
                { line.pop_back(); }
            lines.push_back(line);
            line.clear();
        } else { line += c; }
    }
    if (!line.empty())
        { lines.push_back(line); }
    return true;
}

bool SDCard::fileWrite(std::string filename, std::string message) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);
    message += "\n";

    if (!this->_ensureParentDirs(filename))
        { return false; }
    if (this->_SD->exists(filename.c_str()) && !this->_SD->remove(filename.c_str()))
        { return false; }

    File file = this->_SD->open(filename.c_str(), FILE_WRITE, true);
    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }
    size_t written = file.print(message.c_str());
    file.close();
    return written == message.length();
}

bool SDCard::fileAppend(std::string filename, std::string message) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);
    message += "\n";

    if (!this->_ensureParentDirs(filename))
        { return false; }
    if (!this->_SD->exists(filename.c_str()))
        { return false; }

    File file = this->_SD->open(filename.c_str(), FILE_APPEND, false);
    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }

    size_t written = file.print(message.c_str());
    file.close();

    return written == message.length();
}

bool SDCard::fileWriteOrAppend(std::string filename, std::string message) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);
    message += "\n";

    if (!this->_ensureParentDirs(filename))
        { return false; }

    File file;
    if (this->_SD->exists(filename.c_str()))
        { file = this->_SD->open(filename.c_str(), FILE_APPEND, false); }
    else { file = this->_SD->open(filename.c_str(), FILE_WRITE, true); }

    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }
    size_t written = file.print(message.c_str());
    file.close();
    return written == message.length();
}

bool SDCard::fileCopy(std::string fromFilename, std::string toFilename) {
    if (!this->isInitialized())
        { return false; }
    fromFilename = this->_normalizePath(fromFilename);
    toFilename = this->_normalizePath(toFilename);

    if (!this->fileExists(fromFilename) || this->_SD->exists(toFilename.c_str()))
        { return false; }
    if (!this->_ensureParentDirs(toFilename))
        { return false; }

    File src = this->_SD->open(fromFilename.c_str(), FILE_READ, false);
    File dst = this->_SD->open(toFilename.c_str(), FILE_WRITE, true);
    if (!src || !dst || src.isDirectory() || dst.isDirectory()) {
        if (src) { src.close(); }
        if (dst) { dst.close(); }
        return false;
    }

    uint8_t buffer[512];
    while (src.available()) {
        size_t len = src.read(buffer, sizeof(buffer));
        if (dst.write(buffer, len) != len) {
            src.close();
            dst.close();
            this->_SD->remove(toFilename.c_str());
            return false;
        }
    }
    src.close();
    dst.close();
    return true;
}

bool SDCard::fileErase(std::string filename) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);

    if (!this->_SD->exists(filename.c_str()))
        { return false; }
    if (!this->fileDelete(filename))
        { return false; }

    File file = this->_SD->open(filename.c_str(), FILE_WRITE, true);
    if (file) {
        file.close();
        return true;
    } else { return false; }
}

bool SDCard::fileRename(std::string fromFilename, std::string toFilename) {
    if (!this->isInitialized())
        { return false; }
    fromFilename = this->_normalizePath(fromFilename);
    toFilename = this->_normalizePath(toFilename);

    if (!this->_SD->exists(fromFilename.c_str()))
        { return false; }
    if (this->_SD->exists(toFilename.c_str()))
        { return false; }
    if (!this->_ensureParentDirs(toFilename))
        { return false; }
    return this->_SD->rename(fromFilename.c_str(), toFilename.c_str());
}

bool SDCard::fileDelete(std::string filename) {
    if (!this->isInitialized())
        { return false; }
    filename = this->_normalizePath(filename);
    if (!this->_SD->exists(filename.c_str()))
        { return true; }
    return this->_SD->remove(filename.c_str());
}
