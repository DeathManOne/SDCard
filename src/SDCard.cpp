/*
 * SDCard.cpp
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

SDCard::SDCard() {
    this->_SD = new fs::SDFS(SD);
    this->_INITIALIZED = new bool(false);
}

SDCard::~SDCard() {
    if (*this->_INITIALIZED)
        { this->_SD->end(); }
    delete this->_SD;
    delete this->_INITIALIZED;
}

bool SDCard::cardInfos(uint8_t &type, uint64_t &size, uint64_t &totalBytes, uint64_t &usedBytes) const {
    if (!*this->_INITIALIZED)
        { return false; }
    type = this->_SD->cardType();
    size = this->_SD->cardSize();
    totalBytes = this->_SD->totalBytes();
    usedBytes = this->_SD->usedBytes();

    return type != CARD_NONE;
}

std::string SDCard::_normalizePath(std::string path) {
    if (path.empty() || path[0] != '/')
        { path = "/" + path; }
    return path;
}

std::string SDCard::_joinPath(const std::string &dirname, const char *name) {
    if (dirname == "/")
        { return "/" + std::string(name); }
    return dirname + "/" + std::string(name);
}

bool SDCard::_ensureParentDirs(const std::string &path) {
    if (path.empty())
        { return false; }
    std::string normalized = this->_normalizePath(path);
    size_t pos = 1;

    while ((pos = normalized.find('/', pos)) != std::string::npos) {
        std::string dir = normalized.substr(0, pos);
        if (!dir.empty() && !this->_SD->exists(dir.c_str())) {
            if (!this->_SD->mkdir(dir.c_str()))
                { return false; }
        }
        ++pos;
    }
    return true;
}

bool SDCard::initialize(SPIClass &spi, int cs) {
    *this->_INITIALIZED = this->_SD->begin(cs, spi);
    return *this->_INITIALIZED;
}

std::map<std::string, size_t> SDCard::dirList(std::string dirname, int maxLevel, bool includeDirectories) {
    if (!this->isInitialized())
        { return {}; }
    dirname = this->_normalizePath(dirname);

    std::map<std::string, size_t> tmp = {};
    File dirOrFile = this->_SD->open(dirname.c_str());

    if (!dirOrFile) { return tmp; }
    if (!dirOrFile.isDirectory()) {
        dirOrFile.close();
        return tmp;
    }

    File file = dirOrFile.openNextFile();
    while (file) {
        std::string path = this->_joinPath(dirname, file.name());
        if (file.isDirectory()) {
            if (includeDirectories)
                { tmp.insert({path, 0}); }
            if (maxLevel > 0) {
                std::map<std::string, size_t> subDir =
                    this->dirList(path, maxLevel - 1, includeDirectories);
                for (const auto& dir : subDir)
                    { tmp.insert({dir.first, dir.second}); }
            }
        } else { tmp.insert({path, file.size()}); }
        file.close();
        file = dirOrFile.openNextFile();
    }
    dirOrFile.close();
    return tmp;
}
