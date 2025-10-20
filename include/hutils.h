#ifndef HUTILS_H
#define HUTILS_H

#ifdef _WIN32
    #include <windows.h>
#else
    #include <cstdlib>
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include "sortAlgo.h"

#ifndef HUTIL_API
#define HUTIL_API
#endif  

constexpr int SCREEN_WIDTH  = 100;
constexpr int SCREEN_HEIGHT = 30;

// --- MENUS FUNCTIONS ---
size_t validateChoice(size_t min,
                      size_t max,
                      std::string prompt);
void printDirectories(const SortAlgo &sort,
                      const std::filesystem::path &selSrc);
std::filesystem::path pickSrcDir(const SortAlgo &sort);
void mainMenu   (SortAlgo &sort,
                 const std::string &fileName,
                 std::filesystem::path &selSrc);

namespace hUtils {

    // --- SYSTEM UTILITIES ---
    HUTIL_API void setConsoleWindowSize();          //  Adjust console size.
    HUTIL_API void pause(bool clearBuffer = false); //  Cross-platform system pause.
    HUTIL_API void sleep(int milliseconds);         //  Sleep for a given duration.

    // --- TEXT UTILITIES ---
    struct Text {
    public:
        HUTIL_API void toLine            (char character = '-'); //  Print a line of repeated characters
        HUTIL_API void toCentered        (std::string text,      //  Prints centered text.
                                          int colorCode = 0,
                                          int number = 0,
                                          bool use256 = false);      
        HUTIL_API void toRight           (std::string text,      //  Moves text to the right.
                                          int colorCode = 0,
                                          bool use256 = false);  
        HUTIL_API void toLeft            (std::string text,      //  Moves text to the left.
                                          int colorCode = 0,
                                          int number = 0,
                                          bool use256 = false);
        HUTIL_API std::string toLowerCase(std::string text);     //  Convert string to lowercase.
        HUTIL_API std::string toUpperCase(std::string text);     //  Convert string to uppercase.
        template <typename T>
        HUTIL_API std::string toString(const T& value,           //  Converts int and doubles to string.
                                       int precision)
        {
            std::ostringstream oss;
            if(precision >= 0 && std::is_floating_point<T>::value) {
                oss << std::fixed << std::setprecision(precision);
            }
            oss << value;
            return oss.str();
        }

        HUTIL_API std::string fgColor    (int textColor = 0,     //  Get ANSI color codes.
                                          bool use256 = false);
        HUTIL_API std::string bgColor    (int textColor = 0,
                                          bool use256 = false);
        HUTIL_API std::string defaultText();                     //  Reset text color.

        HUTIL_API void clearAll          ();                     //  Clears every output in the terminal.
        HUTIL_API void clearBelow        (int line);             //  Clears an assigned line below it.
        HUTIL_API void clearAbove        (int line);             //  Clears an assigned line above it.
    };

    // --- LOGGER UTILITIES ---
    struct Logger {
    private:
        int filesMoved = 0, foldersRemoved = 0, foldersCreated = 0, errors = 0, warnings = 0;
    
    public:
        HUTIL_API void Action     (const std::string& action,
                                   const std::string& message);
        HUTIL_API void Success    (const std::string& message);
        HUTIL_API void Moved      (const std::string& message); //  filesMoved
        HUTIL_API void Removed    (const std::string& message); //  filesRemoved
        HUTIL_API void Created    (const std::string& message); //  foldersCreated
        HUTIL_API void Error      (const std::string& message); //  errors
        HUTIL_API void Warning    (const std::string& message); //  warnings
        HUTIL_API void Summary();
    };  
    extern Text   text;
    extern Logger log;      
}

#endif