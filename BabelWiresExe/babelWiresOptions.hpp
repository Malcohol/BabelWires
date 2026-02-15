/**
 * Handle the command-line options for the BabelWires application.
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Utilities/result.hpp>

#include <stdexcept>
#include <string>
#include <ostream>

struct ProgramOptions {
    static babelwires::ResultT<ProgramOptions> parse(int argc, char* argv[]);

    enum Mode { MODE_UI, MODE_DEFAULT = MODE_UI, MODE_PRINT_HELP, MODE_RUN_PROJECT };

    Mode m_mode = MODE_DEFAULT;

    bool m_dumpIsFullDump = false;

    std::string m_inputFileName;
};

void writeUsage(const std::string& programName, std::ostream& stream);
void writeHelp(const std::string& programName, std::ostream& stream);
