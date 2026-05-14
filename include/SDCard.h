/*
 * SDCard.h
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

#ifndef _SD_CARD_
#define _SD_CARD_

#include <Arduino.h>
#include <map>
#include <SD.h>
#include <vector>

class SDCard {
    private:
        fs::SDFS *_SD;
        bool *_INITIALIZED;
        std::string _normalizePath(std::string path);
        std::string _joinPath(const std::string &dirname, const char *name);
        bool _ensureParentDirs(const std::string &path);
    public:
        /**
         * @brief constructor
         */
        SDCard();

        /**
         * @brief disable copy constructor
         */
        SDCard(const SDCard&) = delete;

        /**
         * @brief disable copy assignment
         */
        SDCard& operator=(const SDCard&) = delete;

        /**
         * @brief destructor
         */
        ~SDCard();

        /**
         * @brief check if SD card is initialized
         * @return true if initialized, otherwise false
         */
        inline bool isInitialized() const { return *this->_INITIALIZED; }

        /**
         * @brief get informations about SD card
         * @param type [out] SD card type
         * @param size [out] SD card size
         * @param totalBytes [out] total bytes available
         * @param usedBytes [out] used bytes
         * @return true if card detected, otherwise false
         */
        bool cardInfos(uint8_t &type, uint64_t &size, uint64_t &totalBytes, uint64_t &usedBytes) const;

        /**
         * @brief initialise class, SPI must already be began
         * @param spi SPI class instance
         * @param cs chip select pin
         * @return true if initialised, otherwise false
         */
        bool initialize(SPIClass &spi, int cs);

        /**
         * @brief get a list of files and/or directories
         * @param dirname (default: "/") absolute path of the directory
         * @param maxLevel (default: 0) maximum sub-directories depth
         * @param includeDirectories (default: false) include directories in result
         * @return map<absolute path, file size>
         */
        std::map<std::string, size_t> dirList(std::string dirname = "/", int maxLevel = 0, bool includeDirectories = false);

        /**
         * @brief check if directory exists
         * @param dirname absolute directory path
         * @return true if exists, otherwise false
         */
        bool dirExists(std::string dirname);

        /**
         * @brief create a new directory and missing parent directories
         * @param dirname directory to create
         * @return true if created, otherwise false
         */
        bool dirCreate(std::string dirname);

        /**
         * @brief remove an empty directory
         * @param dirname directory to remove
         * @return true if deleted, otherwise false
         */
        bool dirRemove(std::string dirname);

        /**
         * @brief remove a directory recursively
         * @param dirname directory to remove
         * @return true if deleted, otherwise false
         */
        bool dirRemoveRecursive(std::string dirname);

        /**
         * @brief check if file exists
         * @param filename absolute file path
         * @return true if exists, otherwise false
         */
        bool fileExists(std::string filename);

        /**
         * @brief get file size
         * @param filename absolute file path
         * @return file size in bytes, otherwise 0
         */
        size_t fileSize(std::string filename);

        /**
         * @brief read a file
         * @param filename file to read
         * @param result [out] file content
         * @return true if read, otherwise false
         */
        bool fileRead(std::string filename, std::string &result);

        /**
         * @brief read a file line by line
         * @param filename file to read
         * @param lines [out] file lines
         * @return true if read, otherwise false
         */
        bool fileReadLines(std::string filename, std::vector<std::string> &lines);

        /**
         * @brief create or overwrite a file
         * @param filename file to write
         * @param message message to write
         * @return true if written, otherwise false
         */
        bool fileWrite(std::string filename, std::string message);

        /**
         * @brief append message in existing file
         * @param filename file to append
         * @param message message to append
         * @return true if appended, otherwise false
         */
        bool fileAppend(std::string filename, std::string message);

        /**
         * @brief create and write in new file or append in existing file
         * @param filename file to write or append
         * @param message message to write
         * @return true if created/written or appended, otherwise false
         */
        bool fileWriteOrAppend(std::string filename, std::string message);

        /**
         * @brief copy file
         * @param fromFilename source file
         * @param toFilename destination file
         * @return true if copied, otherwise false
         */
        bool fileCopy(std::string fromFilename, std::string toFilename);

        /**
         * @brief clear all datas in file without deleting it
         * @param filename existing file to erase
         * @return true if erased, otherwise false
         */
        bool fileErase(std::string filename);

        /**
         * @brief rename or move file
         * @param fromFilename existing file
         * @param toFilename new file path
         * @return true if renamed, otherwise false
         */
        bool fileRename(std::string fromFilename, std::string toFilename);

        /**
         * @brief delete file
         * @param filename absolute file path
         * @return true if deleted, otherwise false
         */
        bool fileDelete(std::string filename);
};
#endif