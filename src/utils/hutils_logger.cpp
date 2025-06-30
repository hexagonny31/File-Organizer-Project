#include "hutils.h"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace hUtils;

namespace hUtils {
    
    Logger log;

    void Logger::Action(const string& action, const string& message)
    {
        cout << text.fgColor(93) << action << text.defaultText() << message << '\n';
    }

    void Logger::Success(const string& message)
    {
        cout.flush();
        cout << text.fgColor(92) << message << '\n' << text.defaultText(); 
    }

    void Logger::Moved(const string& message)
    {
        cout.flush();
        cout << text.fgColor(93) << "Moved: " << text.defaultText() << message << '\n'; 
        filesMoved++;
    }

    void Logger::Removed(const string& message)
    {
        cout.flush();
        cout << text.fgColor(93) << "Successfuly deleted: " << text.defaultText() << message << '\n'; 
        foldersRemoved++;
    }

    void Logger::Created(const string& message)
    {
        cout.flush();
        cout << text.fgColor(92) << "New folder created: " << text.defaultText() << message << '\n'; 
        foldersCreated++;
    }

    void Logger::Error(const string& message)
    {
        cerr.flush();
        cerr << text.fgColor(91) << "Error: " << text.defaultText() << message << '\n'; 
        errors++;
    }

    void Logger::Warning(const string& message)
    {
        cerr.flush();
        cerr << text.fgColor(95) << "Warning: " << text.defaultText() << message << '\n'; 
        warnings++;
    }

    void Logger::Summary()
    {
        cout << "\nSummary:\n\n"
             << setw(17) << left << "Files Moved: "     << filesMoved     << '\n'
             << setw(17) << left << "Folders Removed: " << foldersRemoved << '\n'
             << setw(17) << left << "Folders Created: " << foldersCreated << '\n'
             << setw(17) << left << "Warnings: "        << warnings       << '\n'
             << setw(17) << left << "Errors: "          << errors         << "\n\n";
    }
}