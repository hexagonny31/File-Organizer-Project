import file_sorter as sort
import json
from pathlib import Path
from PyQt6.QtWidgets import (QApplication, QHeaderView, QPushButton, QLineEdit, QTableWidgetItem, QTextEdit, QWidget, QLabel, QSizePolicy,
                             QVBoxLayout, QHBoxLayout, QTableWidget, QComboBox, QMessageBox, QFileDialog)
from PyQt6.QtGui import QIcon

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        
        self.config_file = "settings.json"
        self.roots = [] # search roots list. where the program will look for files to move.
        self.targets = [] # target directories list. where the files will be sorted into.
        self.tuples = [("",[])] # list of (folder_name, extensions) touples. the program will sort files into the folder_name in the targetted folder.
        
        self.runtime_map = {}
        
        self.loadData()
        self.initUI()
        
    def loadData(self):
        file_path = Path(self.config_file)
        if file_path.exists():
            with open(file_path, 'r') as f:
                data = json.load(f)
            
            self.roots = [Path(p) for p in data["search_roots"]]
            self.tuples = data["sorting_rules"]
        else:
            self.saveData() # create the file if it doesn't exist.
            
        self.runtime_map = { # flattening tuples for backend operations.
            ext: folder 
            for folder, ext_list in self.tuples 
            for ext in ext_list
        }
    
    def saveData(self):
        str_to_save = [str(p) for p in self.roots]
        with open(self.config_file, 'w') as f:
            json.dump(str_to_save, f, indent=4)
        
    def initUI(self):
        self.setWindowTitle("File Organizer Project")
        self.setWindowIcon(QIcon("lain.ico"))
        self.setFixedSize(700, 550) # width, height
        
        main_layout = QVBoxLayout()
        
        main_layout.addLayout(self.rootPathsLayout()) # where you add/remove search roots.
        main_layout.addLayout(self.sortingSettingsLayout()) # where you set the sorting rules.
        self.setLayout(main_layout)
        
        self.browse_button.setDefault(True)
        self.browse_button.clicked.connect(self.browseFolder)
        self.add_button.setDefault(True)
        self.add_button.clicked.connect(self.addRoot)
        self.remove_button.setDefault(True)
        self.remove_button.clicked.connect(self.removeRoot)
        for path_obj in self.roots:
            self.combo_box.addItem(str(path_obj))

    def rootPathsLayout(self):
        root_layout_add = QHBoxLayout() # source directory add layout
        self.root_path_input = QLineEdit()
        self.root_path_input.setPlaceholderText("Add a search root...")
        
        self.browse_button = QPushButton("&Browse...")
        self.add_button = QPushButton("Add")
        
        root_layout_add.addWidget(self.root_path_input)
        root_layout_add.addWidget(self.browse_button)
        root_layout_add.addWidget(self.add_button)
        
        root_layout_remove = QHBoxLayout() # source directory remove layout
        self.combo_box = QComboBox()
        self.remove_button = QPushButton("Remove")
        self.remove_button.setSizePolicy(
            QSizePolicy.Policy.Fixed, 
            QSizePolicy.Policy.Fixed
        )
        self.remove_button.setStyleSheet("""
            QPushButton {
                background-color: #ffcccc;
            }
        """)
        
        root_layout_remove.addWidget(self.combo_box)
        root_layout_remove.addWidget(self.remove_button)

        root_layout_container = QVBoxLayout()

        root_layout_container.addWidget(QLabel("Manage Search Roots:"))
        root_layout_container.addLayout(root_layout_add)
        root_layout_container.addLayout(root_layout_remove)
        return root_layout_container
    
    def browseFolder(self):
        folder_selected = QFileDialog.getExistingDirectory(self, "Select Directory")
        if folder_selected:
            self.root_path_input.setText(folder_selected)
            
    def addRoot(self):
        raw_str = self.root_path_input.text().strip()
        if not raw_str:
            return
        
        path_obj = Path(raw_str)
        if not path_obj.is_dir():
            QMessageBox.warning(self, "Invalid Path", "This folder doesn't exist!")
            return     
        if path_obj in self.roots:
            QMessageBox.warning(self, "Duplicate Entry", f"The path is already in the list.")
            self.root_path_input.clear()
            return
        
        self.roots.append(path_obj)
        self.combo_box.addItem(str(path_obj))
        self.saveData()
        self.root_path_input.clear()
    
    def removeRoot(self):
        index = self.combo_box.currentIndex()
        if index != -1:
            self.roots.pop(index)
            self.combo_box.removeItem(index)
            self.saveData()

    def sortingSettingsLayout(self):
        self.table = QTableWidget()
        self.table.setColumnCount(2)
        self.table.setHorizontalHeaderLabels(["Folder Name", "Extensions"])
        
        header = self.table.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.ResizeMode.ResizeToContents)
        header.setSectionResizeMode(1, QHeaderView.ResizeMode.Stretch)
        
        self.table.setRowCount(len(self.tuples))
        
        for row, (folder, exts) in enumerate(self.tuples):
            folder_item = QTableWidgetItem(folder)
            exts_item = QTableWidgetItem(", ".join(exts))
            
            self.table.setItem(row, 0, folder_item)
            self.table.setItem(row, 1, exts_item)

        sort_rules_layout_container = QVBoxLayout()
        sort_rules_layout_container.addWidget(QLabel("Sorting Rules:"))
        sort_rules_layout_container.addWidget(self.table)
        return sort_rules_layout_container
    
app = QApplication([])
app.setStyle("windows")
window = MainWindow()
window.show()
app.exec()