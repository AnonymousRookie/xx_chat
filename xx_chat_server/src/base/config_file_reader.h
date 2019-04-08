/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ConfigFileReader
 */

#ifndef BASE_CONFIG_FILE_READER_H
#define BASE_CONFIG_FILE_READER_H

#include <string>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

class ConfigFileReader
{
public:
    ConfigFileReader(const std::string& filename);
    ~ConfigFileReader();

    const rapidjson::Document& GetDom() { return dom_; }

private:
    void LoadFile(const std::string& filename);

private:
    std::string configFileName_;
    rapidjson::Document dom_;
};


#endif  // BASE_CONFIG_FILE_READER_H