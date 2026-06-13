/*
 * directories.cpp
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

bool SDCard::_dirRemoveRecursive(const char *dirname, int depth) {
    if (!this->isInitialized())
        { return false; }
    if (!dirname)
        { return false; }
    if (depth >= MAX_RECURSIVE_DEPTH)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(dirname, path, sizeof(path)))
        { return false; }
    if (strcmp(path, "/") == 0)
        { return false; }
    File dir = this->_SD.open(path);

    if (!dir || !dir.isDirectory()) {
        if (dir) { dir.close(); }
        return false;
    }

    File file = dir.openNextFile();
    while (file) {
        char childPath[MAX_PATH_LENGTH];
        int written;

        if (strcmp(path, "/") == 0)
            { written = snprintf(childPath, sizeof(childPath), "/%s", file.name()); }
        else { written = snprintf(childPath, sizeof(childPath), "%s/%s", path, file.name()); }

        if (written < 0 || static_cast<size_t>(written) >= sizeof(childPath)) {
            file.close();
            dir.close();
            return false;
        }

        bool ok;
        if (file.isDirectory())
            { ok = this->_dirRemoveRecursive(childPath, depth + 1); }
        else { ok = this->_SD.remove(childPath); }

        file.close();
        if (!ok) {
            dir.close();
            return false;
        }
        file = dir.openNextFile();
    }
    dir.close();
    return this->_SD.rmdir(path);
}

bool SDCard::dirList(const char *dirname, int maxLevel, bool includeDirectories, DirListCallback callback, void *userData) {
    if (!this->isInitialized())
        { return false; }
    if (!dirname || !callback)
        { return false; }
    if (maxLevel < 0 || maxLevel > MAX_RECURSIVE_DEPTH)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(dirname, path, sizeof(path)))
        { return false; }
    File dir = this->_SD.open(path);

    if (!dir || !dir.isDirectory()) {
        if (dir) { dir.close(); }
        return false;
    }

    File file = dir.openNextFile();
    while (file) {
        char childPath[MAX_PATH_LENGTH];
        int written;

        if (strcmp(path, "/") == 0)
            { written = snprintf(childPath, sizeof(childPath), "/%s", file.name()); }
        else { written = snprintf(childPath, sizeof(childPath), "%s/%s", path, file.name()); }

        if (written < 0 || static_cast<size_t>(written) >= sizeof(childPath)) {
            file.close();
            dir.close();
            return false;
        }

        bool isDirectory = file.isDirectory();
        size_t size = isDirectory ? 0 : file.size();
        if (!isDirectory || includeDirectories) {
            if (!callback(childPath, size, isDirectory, userData)) {
                file.close();
                dir.close();
                return true;
            }
        }

        if (isDirectory && maxLevel > 0) {
            file.close();
            if (!this->dirList(childPath, maxLevel - 1, includeDirectories, callback, userData)) {
                dir.close();
                return false;
            }
        } else { file.close(); }
        file = dir.openNextFile();
    }
    dir.close();
    return true;
}

bool SDCard::dirExists(const char *dirname) {
    if (!this->isInitialized())
        { return false; }
    if (!dirname)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(dirname, path, sizeof(path)))
        { return false; }
    File file = this->_SD.open(path);

    if (!file)
        { return false; }
    bool result = file.isDirectory();
    file.close();
    return result;
}

bool SDCard::dirCreate(const char *dirname) {
    if (!this->isInitialized())
        { return false; }
    if (!dirname)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(dirname, path, sizeof(path)))
        { return false; }
    if (this->_SD.exists(path))
        { return this->dirExists(path); }
    char parentPath[MAX_PATH_LENGTH];

    if (snprintf(parentPath, sizeof(parentPath), "%s/", path) >= sizeof(parentPath))
        { return false; }
    if (!this->_ensureParentDirs(parentPath))
        { return false; }
    return this->_SD.mkdir(path);
}

bool SDCard::dirRemove(const char *dirname) {
    if (!this->isInitialized())
        { return false; }
    if (!dirname)
        { return false; }
    char path[MAX_PATH_LENGTH];

    if (!this->_normalizePath(dirname, path, sizeof(path)))
        { return false; }
    if (strcmp(path, "/") == 0)
        { return false; }
    if (!this->_SD.exists(path))
        { return true; }
    return this->_SD.rmdir(path);
}
