/*
 * src/files_write_extended.cpp
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

#include "SDCard.h"

bool SDCard::fileWrite(const char *filename, const uint8_t *buffer, size_t length) {
    if (!this->isInitialized())              { return false; }
    if (!filename || !buffer || length == 0) { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }

    File file = this->_SD.open(path, FILE_WRITE, true);
    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }

    size_t written = file.write(buffer, length);
    file.close();

    return written == length;
}

bool SDCard::fileAppend(const char *filename, const uint8_t *buffer, size_t length) {
    if (!this->isInitialized())              { return false; }
    if (!filename || !buffer || length == 0) { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }
    if (!this->_SD.exists(path))                             { return false; }

    File file = this->_SD.open(path, FILE_APPEND, false);
    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }

    size_t written = file.write(buffer, length);
    file.close();

    return written == length;
}

bool SDCard::fileWriteOrAppend(const char *filename, const uint8_t *buffer, size_t length) {
    if (!this->isInitialized())              { return false; }
    if (!filename || !buffer || length == 0) { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }

    File file;
    if (this->_SD.exists(path))
        { file  = this->_SD.open(path, FILE_APPEND, false); }
    else { file = this->_SD.open(path, FILE_WRITE,  true); }

    if (!file || file.isDirectory()) {
        if (file)
            { file.close(); }
        return false;
    }

    size_t written = file.write(buffer, length);
    file.close();

    return written == length;
}

bool SDCard::fileWriteOpen(const char *filename) {
    if (!this->isInitialized() || !filename) { return false; }
    if (this->_WRITE_FILE)                   { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }

    this->_WRITE_FILE = this->_SD.open(path, FILE_WRITE, true);
    if (!this->_WRITE_FILE || this->_WRITE_FILE.isDirectory()) {
        if (this->_WRITE_FILE)
            { this->_WRITE_FILE.close(); }
        return false;
    }

    return true;
}

bool SDCard::fileWriteChunk(const uint8_t *buffer, size_t length) {
    if (!this->isInitialized() || !this->_WRITE_FILE) { return false; }
    if (!buffer || length == 0)                       { return false; }
    if (this->_WRITE_FILE.isDirectory())              { return false; }

    return this->_WRITE_FILE.write(buffer, length) == length;
}

void SDCard::fileWriteClose() {
    if (!this->_WRITE_FILE) { return; }

    this->_WRITE_FILE.flush();
    this->_WRITE_FILE.close();
}
