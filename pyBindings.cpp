#include "sortAlgo.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

namespace py = pybind11;

PYBIND11_MODULE(file_sorter, m) {
    m.doc() = "A module for sorting files by extension or alphabetically";
    m.def("move_file", &moveFile,
          "Move a file to a destination directory",
          py::arg("entry"), py::arg("destination"));
    m.def("to_src_dir", &toSrcDir,
          "Move files to a targeted source directory",
          py::arg("src"), py::arg("init_dir"), py::arg("dest_map"));
    m.def("by_ext", &byExt,
          "Sort files by extension",
          py::arg("src"), py::arg("dest_map"));
    m.def("by_alph", &byAlph,
          "Sort files alphabetically",
          py::arg("src"));
    m.def("remove_folder", &removeFolder,
          "Remove a folder and all its contents",
          py::arg("destination"));
    m.def("remove_ext", &removeExt,
          "Remove files by extension",
          py::arg("src"), py::arg("dest_map"));
    m.def("remove_alph", &removeAlph,
          "Remove files alphabetically",
          py::arg("src"));
    m.def("build_dest_map", &buildDestMap,
          "Build a mapping of file extensions to destination directories",
          py::arg("src"), py::arg("keys"), py::arg("log_print") = true);

    m.def("set_log_callbacks", [](py::function info, py::function error, py::function warn) {
        py_log_info  = [info] (const std::string& s) { info(s); };
        py_log_error = [error](const std::string& s) { error(s); };
        py_log_warn  = [warn] (const std::string& s) { warn(s); };
    });
    m.def("set_conflict_callback", [](py::function cd) {
        py_conflict_decision = [cd](const std::string& name) -> int {
            return cd(name).cast<int>();
        };
    });
}