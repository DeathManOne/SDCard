/*
 * src/files_manage.cpp
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

bool SDCard::fileCopy(const char *fromFilename, const char *toFilename) {
    if (!this->isInitialized())       { return false; }
    if (!fromFilename || !toFilename) { return false; }

    char fromPath[MAX_PATH_LENGTH];
    char toPath[MAX_PATH_LENGTH];

    if (!this->_normalizePath(fromFilename, fromPath, sizeof(fromPath))) { return false; }
    if (!this->_normalizePath(toFilename,   toPath,   sizeof(toPath)))   { return false; }
    if (strcmp(fromPath, toPath) == 0)                                   { return false; }
    if (!this->fileExists(fromPath) || this->_SD.exists(toPath))         { return false; }
    if (!this->_ensureParentDirs(toPath))                                { return false; }

    File src = this->_SD.open(fromPath, FILE_READ,  false);
    File dst = this->_SD.open(toPath,   FILE_WRITE, true);

    if (!src || !dst || src.isDirectory() || dst.isDirectory()) {
        if (src) { src.close(); }
        if (dst) { dst.close(); }
        if (this->_SD.exists(toPath))
            { this->_SD.remove(toPath); }
        return false;
    }

    uint8_t buffer[FILE_BUFFER_SIZE];
    size_t copiedSize         = 0;
    const size_t expectedSize = src.size();

    while (copiedSize < expectedSize) {
        size_t len = src.read(buffer, sizeof(buffer));
        if (len == 0 || dst.write(buffer, len) != len) {
            src.close();
            dst.close();
            this->_SD.remove(toPath);
            return false;
        }
        copiedSize += len;
    }

    dst.flush();
    const bool complete =
        copiedSize == expectedSize &&
        dst.size() == expectedSize;

    src.close();
    dst.close();

    if (!complete) {
        this->_SD.remove(toPath);
        return false;
    }
    return true;
}

bool SDCard::fileErase(const char *filename) {
    if (!this->isInitialized()) { return false; }
    if (!filename)              { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->fileExists(path))                             { return false; }

    File file = this->_SD.open(path, FILE_WRITE, false);
    if (!file) { return false; }

    if (file.isDirectory()) {
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool SDCard::fileRename(const char *fromFilename, const char *toFilename) {
    if (!this->isInitialized())       { return false; }
    if (!fromFilename || !toFilename) { return false; }

    char fromPath[MAX_PATH_LENGTH];
    char toPath[MAX_PATH_LENGTH];

    if (!this->_normalizePath(fromFilename, fromPath, sizeof(fromPath))) { return false; }
    if (!this->_normalizePath(toFilename,   toPath,   sizeof(toPath)))   { return false; }
    if (!this->fileExists(fromPath))                                     { return false; }
    if (this->_SD.exists(toPath))                                        { return false; }
    if (!this->_ensureParentDirs(toPath))                                { return false; }

    return this->_SD.rename(fromPath, toPath);
}

bool SDCard::fileDelete(const char *filename) {
    if (!this->isInitialized()) { return false; }
    if (!filename)              { return false; }

    char path[MAX_PATH_LENGTH];
    if (!this->_normalizePath(filename, path, sizeof(path))) { return false; }
    if (!this->_SD.exists(path))                             { return true; }

    return this->_SD.remove(path);
}
