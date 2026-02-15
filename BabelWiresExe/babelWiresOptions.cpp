/**
 * Handle the command-line options for the BabelWires application.
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresExe/babelWiresOptions.hpp>

#include <BaseLib/exceptions.hpp>

namespace {
    const char s_helpString[] = "help";
    const char s_runString[] = "run";
    const char s_uiString[] = "ui";
} // namespace

babelwires::ResultT<ProgramOptions> ProgramOptions::parse(int argc, char* argv[]) {
    ProgramOptions options;
    if (argc == 1) {
        // UI mode.
        return options;
    }

    const std::string modeArg = argv[1];

    if ((modeArg == s_helpString) || (modeArg == "-h") || (modeArg == "--help")) {
        options.m_mode = ProgramOptions::MODE_PRINT_HELP;
    } else if (modeArg == s_runString) {
        if (argc != 3) {
            return babelwires::Error() << "Wrong number of arguments for " << s_runString << " mode";
        }
        options.m_mode = ProgramOptions::MODE_RUN_PROJECT;
        options.m_inputFileName = argv[2];
    } else if (modeArg != s_uiString) {
        return babelwires::Error() << "Unrecognized mode \"" << modeArg << "\" provided";
    }
    return options;
}

void writeUsage(const std::string& programName, std::ostream& stream) {
    stream << "Usage:" << std::endl;
    stream << programName << std::endl;
    stream << programName << " " << s_runString << " projectFile" << std::endl;
    stream << programName << " " << s_helpString << std::endl;
}

void writeHelp(const std::string& programName, std::ostream& stream) {
    stream << programName << " - A program to transform music sequence data between various file formats." << std::endl;
    writeUsage(programName, stream);
}
