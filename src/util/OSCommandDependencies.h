//
// Created by NavKav on 25/05/2025.
//

#ifndef OSCOMMANDUTILS_H
#define OSCOMMANDUTILS_H

#include <cstdlib>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
inline void launchServerConsole(const char* executablePath) {
    std::string cmd = "start \"STSalike Server console\" cmd /k \"" + std::string(executablePath) + " _server < nul\"";
    system(cmd.c_str());
}
#elif defined(__linux__)
inline void launchServerConsole(const char* executablePath) {
    std::string cmd =
        "gnome-terminal -- bash -c '" + std::string(executablePath) + " _server; exec bash'";
    system(cmd.c_str());
}
#elif defined(__APPLE__) && defined(__MACH__)
inline void launchServerConsole(const char* executablePath) {
    std::string cmd =
        "osascript -e 'tell application \"Terminal\" to do script \"" + std::string(executablePath) + " _server\"'";
    system(cmd.c_str());
}
#else
#error Unsupported operating system
#endif


#endif //OSCOMMANDUTILS_H
