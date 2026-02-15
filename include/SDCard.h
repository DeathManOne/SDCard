#ifndef _SD_CARD_
#define _SD_CARD_

#include <Arduino.h>
#include <map>
#include <SD.h>

/**
 * @brief work with SPI
 */
class SDCard {
    private:
        fs::SDFS *_SD;
    public:
        /**
         * @brief constructor
         */
        SDCard();

        /**
         * @brief destructor
         */
        ~SDCard();

        /**
         * @brief initialise class, SPI must be already began
         * @param SDA chip select
         * @param SPI needed form SPI.h
         * @return true if initialised, otherwise false
         */
        bool initialize(int SDA, SPIClass &SPI) const;

        /**
         * @brief get a list of files, if a directory is empty, it will not seen
         * @param dirname (default: "/") absolute path of the directory
         * @param maxLevel (default: 0) how much sub-directories would you like to see
         * @return map<absolute file path, file size>
         */
        std::map<std::string, size_t> dirList(std::string dirname = "/", int maxLevel = 0) const;

        /**
         * @brief check whether a file or directory exist
         * @param dirnameOrFilename name of the file or directory
         * @return true if exist, otherwise false
         */
        bool dirOrFileExist(std::string dirnameOrFilename) const;

        /**
         * @brief create a new directory
         * @param dirname name of the new directory
         * @return true if created, otherwise false
         */
        bool dirCreate(std::string dirname) const;

        /**
         * @brief remove existing directory
         * @param dirname directory to remove
         * @return true if deleted, otherwise false
         */
        bool dirRemove(std::string dirname) const;

        /**
         * @brief read a file
         * @param filename file to read
         * @param result [out]
         * @return true if read, otherwise false
         */
        bool fileRead(std::string filename, std::string &result) const;

        /**
         * @brief create and write in new file or append in existing file
         * @param filename file to write or append
         * @param message write your message to write
         * @return true if create/write or append, otherwise false
         */
        bool fileWriteOrAppend(std::string filename, std::string message) const;

        /**
         * @brief clear all datas in file without delete the file
         * @param filename existing file to erase
         * @return true if erased, otherwise false
         */
        bool fileErase(std::string filename) const;

        /**
         * @brief rename file
         * @param fromFilename existing file to rename
         * @param toFilename new name, must not already exist
         * @return true if renamed, otherwise false
         */
        bool fileRename(std::string fromFilename, std::string toFilename) const;

        /**
         * @brief delete file
         * @param filename absolute path to file
         * @return true if file deleted, otherwise false
         */
        bool fileDelete(std::string filename) const;
};
#endif