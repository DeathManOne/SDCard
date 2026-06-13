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

#pragma once
#include <SD.h>

class SDCard {
    private:
        bool _INITIALIZED = false;
        fs::SDFS _SD = fs::SDFS(SD);
        bool _normalizePath(const char *input, char *output, size_t outputSize);
        bool _ensureParentDirs(const char *path);
        bool _dirRemoveRecursive(const char *dirname, int depth);
    public:
        /**
         * @brief library limits and buffer sizes
         */
        static constexpr int MAX_RECURSIVE_DEPTH = 16;
        static constexpr size_t MAX_PATH_LENGTH = 256;
        static constexpr size_t MAX_LINE_LENGTH = 512;
        static constexpr size_t FILE_BUFFER_SIZE = 512;

        /**
         * @brief callback types
         */
        typedef bool (*FileReadCallback)(const uint8_t *buffer, size_t length, void *userData);
        typedef bool (*FileReadLineCallback)(const char *line, void *userData);
        typedef bool (*DirListCallback)(const char *path, size_t size, bool isDirectory, void *userData);

        /**
         * @brief constructor
         */
        SDCard() {}

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
        inline bool isInitialized() const { return this->_INITIALIZED; }

        /**
         * @brief get SD card informations
         * @param type [out] SD card type
         * @param size [out] SD card size in bytes
         * @param totalBytes [out] total filesystem size in bytes
         * @param usedBytes [out] used filesystem size in bytes
         * @return true if card detected, otherwise false
         */
        bool cardInfos(uint8_t &type, uint64_t &size, uint64_t &totalBytes, uint64_t &usedBytes);

        /**
         * @brief initialize SD card
         * @param spi initialized SPI instance
         * @param cs chip select pin
         * @return true if initialized, otherwise false
         */
        bool initialize(SPIClass &spi, int cs);

        /**
         * @brief list files and/or directories using a callback
         * @param dirname directory to explore
         * @param maxLevel maximum sub-directory depth
         * @param includeDirectories include directories in callback results
         * @param callback called for each file or directory, return false to stop listing
         * @param userData optional user context passed to callback
         * @return true if listed, otherwise false
         */
        bool dirList(const char *dirname, int maxLevel, bool includeDirectories, DirListCallback callback, void *userData = nullptr);

        /**
         * @brief check if directory exists
         * @param dirname absolute directory path
         * @return true if exists, otherwise false
         */
        bool dirExists(const char *dirname);

        /**
         * @brief create a directory and missing parent directories
         * @param dirname directory to create
         * @return true if created or already exists, otherwise false
         * @note Missing parent directories are created automatically
         */
        bool dirCreate(const char *dirname);

        /**
         * @brief remove an empty directory
         * @param dirname directory to remove
         * @return true if removed or does not exist, otherwise false
         */
        bool dirRemove(const char *dirname);

        /**
         * @brief remove a directory recursively
         * @param dirname directory to remove
         * @return true if removed, otherwise false
         * @note Root directory (/) cannot be removed
         */
        inline bool dirRemoveRecursive(const char *dirname) { return this->_dirRemoveRecursive(dirname, 0); }

        /**
         * @brief check if file exists
         * @param filename absolute file path
         * @return true if exists, otherwise false
         */
        bool fileExists(const char *filename);

        /**
         * @brief get file size
         * @param filename absolute file path
         * @return file size in bytes, otherwise 0
         */
        size_t fileSize(const char *filename);

        /**
         * @brief read a file by chunks
         * @param filename file to read
         * @param callback called for each read chunk, return false to stop reading
         * @param userData optional user context passed to callback
         * @return true if read, otherwise false
         */
        bool fileRead(const char *filename, FileReadCallback callback, void *userData = nullptr);

        /**
         * @brief read a file line by line
         * @param filename file to read
         * @param callback called for each line, return false to stop reading
         * @param userData optional user context passed to callback
         * @return true if read, otherwise false
         */
        bool fileReadLines(const char *filename, FileReadLineCallback callback, void *userData = nullptr);

        /**
         * @brief create or overwrite a text file
         * @param filename file to write
         * @param message null-terminated message to write
         * @param addNewLine add a newline character at the end of message
         * @return true if written, otherwise false
         */
        bool fileWrite(const char *filename, const char *message, bool addNewLine = false);

        /**
         * @brief append text to an existing file
         * @param filename file to append
         * @param message null-terminated message to append
         * @param addNewLine add a newline character at the end of message
         * @return true if appended, otherwise false
         */
        bool fileAppend(const char *filename, const char *message, bool addNewLine = false);

        /**
         * @brief create and write text in new file or append text in existing file
         * @param filename file to write or append
         * @param message null-terminated message to write or append
         * @param addNewLine add a newline character at the end of message
         * @return true if created/written or appended, otherwise false
         */
        bool fileWriteOrAppend(const char *filename, const char *message, bool addNewLine = false);

        /**
         * @brief write raw data to a file
         * @param filename file to write
         * @param buffer data buffer to write
         * @param length number of bytes to write
         * @return true if written, otherwise false
         * @note Ideal for binary data or text containing null bytes (`\0`)
         */
        bool fileWrite(const char *filename, const uint8_t *buffer, size_t length);

        /**
         * @brief append raw data to an existing file
         * @param filename file to append
         * @param buffer data buffer to append
         * @param length number of bytes to append
         * @return true if appended, otherwise false
         * @note Ideal for binary data or text containing null bytes (`\0`)
         */
        bool fileAppend(const char *filename, const uint8_t *buffer, size_t length);

        /**
         * @brief create and write raw data in new file or append raw data in existing file
         * @param filename file to write or append
         * @param buffer data buffer
         * @param length number of bytes to write
         * @return true if created/written or appended, otherwise false
         * @note Ideal for binary data or text containing null bytes (`\0`)
         */
        bool fileWriteOrAppend(const char *filename, const uint8_t *buffer, size_t length);

        /**
         * @brief copy a file
         * @param fromFilename source file
         * @param toFilename destination file
         * @return true if copied, otherwise false
         * @note Missing parent directories are created automatically
         */
        bool fileCopy(const char *fromFilename, const char *toFilename);

        /**
         * @brief clear all data in a file without deleting it
         * @param filename existing file to erase
         * @return true if erased, otherwise false
         */
        bool fileErase(const char *filename);

        /**
         * @brief rename or move a file
         * @param fromFilename existing file
         * @param toFilename new file path
         * @return true if renamed, otherwise false
         * @note Missing parent directories are created automatically
         */
        bool fileRename(const char *fromFilename, const char *toFilename);

        /**
         * @brief delete a file
         * @param filename file to delete
         * @return true if deleted or does not exist, otherwise false
         */
        bool fileDelete(const char *filename);
};
