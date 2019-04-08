/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ConfigFileReader
 */

#include "stdafx.h"
#include "config_file_reader.h"

ConfigFileReader::ConfigFileReader(const std::string& filename)
    : configFileName_(filename)
{
    LoadFile(filename);
}

ConfigFileReader::~ConfigFileReader()
{

}

void ConfigFileReader::LoadFile(const std::string& filename)
{
    std::ifstream ifs(filename.c_str());
    if (!ifs) {
        printf("open %s failed!", filename.c_str());
        return;
    }
    rapidjson::IStreamWrapper isw(ifs);
    dom_.ParseStream(isw);
}
