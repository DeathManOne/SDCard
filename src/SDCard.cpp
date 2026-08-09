/*
 * src/SDCard.cpp
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

bool SDCard::_normalizePath(const char *input, char *output, size_t outputSize) {
    if (!input || input[0] == '\0' || !output || outputSize < 2) { return false; }

    const char *segment = input;
    while (*segment != '\0') {
        while (*segment == '/')
            { ++segment; }

        const char *end = segment;
        while (*end != '\0' && *end != '/')
            { ++end; }

        const size_t length = static_cast<size_t>(end - segment);
        if ((length == 1U && segment[0] == '.') ||
            (length == 2U && segment[0] == '.' && segment[1] == '.')
        ) { return false; }
        segment = end;
    }

    size_t out        = 0;
    bool lastWasSlash = false;

    if (input[0] != '/') {
        output[out++] = '/';
        lastWasSlash  = true;
    }

    for (size_t i = 0; input[i] != '\0'; ++i) {
        char c = input[i];

        if (c == '/') {
            if (lastWasSlash)
                { continue; }
            lastWasSlash = true;
        } else { lastWasSlash = false; }

        if (out + 1 >= outputSize)
            { return false; }
        output[out++] = c;
    }

    while (out > 1 && output[out - 1] == '/')
        { --out; }
    output[out] = '\0';

    return true;
}

bool SDCard::_ensureParentDirs(const char *path) {
    if (!path) { return false; }

    char normalized[MAX_PATH_LENGTH];
    if (!this->_normalizePath(path, normalized, sizeof(normalized))) { return false; }

    size_t len = strlen(normalized);
    if (len == 0) { return false; }

    char dir[MAX_PATH_LENGTH];
    for (size_t i = 1; i < len; ++i) {
        if (normalized[i] != '/') { continue; }
        if (i >= sizeof(dir))     { return false; }

        memcpy(dir, normalized, i);
        dir[i] = '\0';

        if (!this->_SD.exists(dir)) {
            if (!this->_SD.mkdir(dir))
                { return false; }
        }
    }

    return true;
}

SDCard::~SDCard() {
    this->fileWriteClose();

    if (this->_INITIALIZED) { this->_SD.end(); }
    this->_INITIALIZED = false;
}

bool SDCard::cardInfos(uint8_t &type, uint64_t &size, uint64_t &totalBytes, uint64_t &usedBytes) {
    if (!this->_INITIALIZED) { return false; }

    type       = this->_SD.cardType();
    size       = this->_SD.cardSize();
    totalBytes = this->_SD.totalBytes();
    usedBytes  = this->_SD.usedBytes();

    return type != CARD_NONE;
}

bool SDCard::initialize(SPIClass &spi, int cs) {
    if (this->_INITIALIZED) { return true; }

    this->_INITIALIZED = this->_SD.begin(cs, spi);
    return this->_INITIALIZED;
}
