/*
 * src/files_write_basic.cpp
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

bool SDCard::fileWrite(const char *filename, const char *message, bool addNewLine) {
    if (!this->isInitialized()) { return false; }
    if (!filename || !message)  { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }

    File file = this->_SD.open(path, FILE_WRITE, true);
    if (!file || file.isDirectory()) {
        if (file) { file.close(); }
        return false;
    }

    size_t messageLength = strlen(message);
    size_t written       = file.write(reinterpret_cast<const uint8_t *>(message), messageLength);
    if (addNewLine) { written += file.write('\n'); }

    file.close();
    return written == messageLength + (addNewLine ? 1 : 0);
}

bool SDCard::fileAppend(const char *filename, const char *message, bool addNewLine) {
    if (!this->isInitialized()) { return false; }
    if (!filename || !message)  { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }
    if (!this->_SD.exists(path))                             { return false; }

    File file = this->_SD.open(path, FILE_APPEND, false);
    if (!file || file.isDirectory()) {
        if (file) { file.close(); }
        return false;
    }

    size_t messageLength = strlen(message);
    size_t written       = file.write(reinterpret_cast<const uint8_t *>(message), messageLength);
    if (addNewLine) { written += file.write('\n'); }

    file.close();
    return written == messageLength + (addNewLine ? 1 : 0);
}

bool SDCard::fileWriteOrAppend(const char *filename, const char *message, bool addNewLine) {
    if (!this->isInitialized()) { return false; }
    if (!filename || !message)  { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_ensureParentDirs(path))                      { return false; }

    File file;
    if (this->_SD.exists(path))
        { file  = this->_SD.open(path, FILE_APPEND, false); }
    else { file = this->_SD.open(path, FILE_WRITE,  true); }

    if (!file || file.isDirectory()) {
        if (file) { file.close(); }
        return false;
    }

    size_t messageLength = strlen(message);
    size_t written       = file.write(reinterpret_cast<const uint8_t *>(message), messageLength);
    if (addNewLine) { written += file.write('\n'); }

    file.close();
    return written == messageLength + (addNewLine ? 1 : 0);
}
