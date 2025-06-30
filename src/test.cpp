#include <iostream>

using std::cout;
using std::string;

string fgColor(int textColor, bool use256 = false)
{
    if(use256){
        // 256-color mode (0-255)
        if (textColor >= 0 && textColor <= 255){
            return "\033[38;5;" + std::to_string(textColor) + "m";
        }
    } 
    else{
        // Standard ANSI 16 colors
        if ((textColor >= 30 && textColor <= 37) || (textColor >= 90 && textColor <= 97)){
            return "\033[" + std::to_string(textColor) + "m";
        }
    }

    return "";
}

string bgColor(int textColor, bool use256 = false)
{
    if(use256){
        // 256-color mode (0-255)
        if (textColor >= 0 && textColor <= 255){
            return "\033[48;5;" + std::to_string(textColor) + "m";
        }
    }
    else{
        // Standard ANSI 16 colors
        if ((textColor >= 40 && textColor <= 47) || (textColor >= 100 && textColor <= 107)){
            return "\033[" + std::to_string(textColor) + "m";
        }
    }

    return "";
}

string defaultText()
{
    return "\033[0m";
}

int main() {
    cout << bgColor(104, true) << fgColor(196, true) << "Hello World" << defaultText();
}