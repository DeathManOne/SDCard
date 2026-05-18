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

#include "../include/SDCard.h"

bool SDCard::dirExists(std::string dirname) {
    if (!this->isInitialized())
        { return false; }
    dirname = this->_normalizePath(dirname);
    File file = this->_SD.open(dirname.c_str());
    if (!file)
        { return false; }
    bool result = file.isDirectory();
    file.close();
    return result;
}

bool SDCard::dirCreate(std::string dirname) {
    if (!this->isInitialized())
        { return false; }
    dirname = this->_normalizePath(dirname);

    if (this->_SD.exists(dirname.c_str()))
        { return this->dirExists(dirname); }
    return this->_ensureParentDirs(dirname + "/");
}

bool SDCard::dirRemove(std::string dirname) {
    if (!this->isInitialized())
        { return false; }
    if (dirname == "/")
        { return false; }
    dirname = this->_normalizePath(dirname);

    if (!this->_SD.exists(dirname.c_str()))
        { return true; }
    return this->_SD.rmdir(dirname.c_str());
}

bool SDCard::dirRemoveRecursive(std::string dirname) {
    if (!this->isInitialized())
        { return false; }
    if (dirname == "/")
        { return false; }
    dirname = this->_normalizePath(dirname);

    File dir = this->_SD.open(dirname.c_str());
    if (!dir || !dir.isDirectory()) {
        if (dir)
            { dir.close(); }
        return false;
    }

    File file = dir.openNextFile();
    while (file) {
        std::string path = this->_joinPath(dirname, file.name());
        bool ok;

        if (file.isDirectory())
            { ok = this->dirRemoveRecursive(path); }
        else { ok = this->_SD.remove(path.c_str()); }

        file.close();
        if (!ok) {
            dir.close();
            return false;
        }
        file = dir.openNextFile();
    }
    dir.close();
    return this->_SD.rmdir(dirname.c_str());
}
