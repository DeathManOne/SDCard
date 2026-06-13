/*
 * files_read.cpp
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

bool SDCard::fileExists(const char *filename) {
    if (!this->isInitialized())
        { return false; }
    if (!filename)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(filename, path, sizeof(path)))
        { return false; }
    File file = this->_SD.open(path);

    if (!file)
        { return false; }
    bool result = !file.isDirectory();
    file.close();
    return result;
}

size_t SDCard::fileSize(const char *filename) {
    if (!this->isInitialized())
        { return 0; }
    if (!filename)
        { return 0; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(filename, path, sizeof(path)))
        { return 0; }
    File file = this->_SD.open(path, FILE_READ, false);

    if (!file || file.isDirectory()) {
        if (file) { file.close(); }
        return 0;
    }

    size_t result = file.size();
    file.close();
    return result;
}

bool SDCard::fileRead(const char *filename, FileReadCallback callback, void *userData) {
    if (!this->isInitialized())
        { return false; }
    if (!filename || !callback)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(filename, path, sizeof(path)))
        { return false; }
    if (!this->_SD.exists(path))
        { return false; }
    File file = this->_SD.open(path, FILE_READ, false);

    if (!file || file.isDirectory()) {
        if (file) { file.close(); }
        return false;
    }

    uint8_t buffer[FILE_BUFFER_SIZE];
    while (file.available()) {
        size_t len = file.read(buffer, sizeof(buffer));
        if (len == 0) {
            file.close();
            return false;
        }
        if (!callback(buffer, len, userData)) {
            file.close();
            return true;
        }
    }
    file.close();
    return true;
}

bool SDCard::fileReadLines(const char *filename, FileReadLineCallback callback, void *userData) {
    if (!this->isInitialized())
        { return false; }
    if (!filename || !callback)
        { return false; }
    char line[MAX_LINE_LENGTH];
    size_t pos = 0;

    struct Context {
        char *line;
        size_t *pos;
        FileReadLineCallback callback;
        void *userData;
    } context = {line, &pos, callback, userData};

    bool ok = this->fileRead(
        filename, [](const uint8_t *buffer, size_t length, void *data) -> bool {
            Context *ctx = static_cast<Context *>(data);

            for (size_t i = 0; i < length; ++i) {
                char c = static_cast<char>(buffer[i]);
                if (c == '\r')
                    { continue; }
                if (c == '\n') {
                    ctx->line[*ctx->pos] = '\0';
                    if (!ctx->callback(ctx->line, ctx->userData))
                        { return false; }
                    *ctx->pos = 0;
                    continue;
                }

                if (*ctx->pos + 1 >= SDCard::MAX_LINE_LENGTH)
                    { return false; }
                ctx->line[(*ctx->pos)++] = c;
            }
            return true;
        },
        &context
    );

    if (!ok)
        { return false; }
    if (pos > 0) {
        line[pos] = '\0';
        return callback(line, userData);
    }
    return true;
}
