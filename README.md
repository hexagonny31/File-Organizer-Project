# File Organizer Program
A simple file management program using **C++ backend** for quick file mapping and with **PyQt6** for frontend interface for an intuitive user experience.
## Features
- **Search Roots:** Monitor multiple directories for files to organize.
- **Custom Rules:** Define sorting logic using a dynamic table (Extension ↔ Folder).
- **C++ Engine:** Backend logic used by `std::unordered_map` and `std::filesystem` for efficiency.
- **Simple UI:** Clean, responsive interface with Windows-native styling and dropdown action menus.
## Architecture
The project uses pybind11 to bridge the gap between Python and C++.
- **Frontend:** Python 3.x + PyQt6
- **Backend:** C++17 (compiled as a Python module)
- **Data Persistence:** JSON-based configuration (settings.json)
