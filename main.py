import file_sorter as sort
import json
from pathlib import Path
from PyQt6.QtWidgets import (QApplication, QPushButton, QLineEdit, QTextEdit, QWidget, QLabel, QSizePolicy,
                             QVBoxLayout, QHBoxLayout, QGridLayout, QComboBox, QMessageBox, QFileDialog)
from PyQt6.QtGui import QIcon

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        
        self.config_file = "settings.json"
        self.source_dir = []
        
        self.loadData()
        self.initUI()
        
    def loadData(self):
        file_path = Path(self.config_file)
        if file_path.exists():
            with open(file_path, 'r') as f:
                data = json.load(f)
                self.source_dir = [Path(p) for p in data]
    
    def saveData(self):
        str_to_save = [str(p) for p in self.source_dir]
        with open(self.config_file, 'w') as f:
            json.dump(str_to_save, f, indent=4)
        
    def initUI(self):
        self.setWindowTitle("File Organizer Project")
        self.setWindowIcon(QIcon("lain.ico"))
        self.resize(550, 400) # width, height
        
        main_layout = QVBoxLayout()
        
        src_layout_add = QHBoxLayout()
        self.src_path_input = QLineEdit()
        self.src_path_input.setPlaceholderText("Select a folder...")
        
        self.browse_button = QPushButton("&Browse...")
        self.add_button = QPushButton("Add")
        
        src_layout_add.addWidget(self.src_path_input)
        src_layout_add.addWidget(self.browse_button)
        src_layout_add.addWidget(self.add_button)
        
        src_layout_remove = QHBoxLayout()
        self.combo_box = QComboBox()
        self.remove_button = QPushButton("Remove Selected")
        self.remove_button.setSizePolicy(
            QSizePolicy.Policy.Fixed, 
            QSizePolicy.Policy.Fixed
        )
        self.remove_button.setStyleSheet("""
            QPushButton {
                background-color: #ffcccc;
                padding-left: 10px;
                padding-right: 10px;
                padding-top: 3px;
                padding-bottom: 3px;
            }
        """)
        
        src_layout_remove.addWidget(self.combo_box)
        src_layout_remove.addWidget(self.remove_button)

        src_layout_container = QVBoxLayout()

        src_layout_container.addLayout(src_layout_add)
        src_layout_container.addLayout(src_layout_remove)
        main_layout.addWidget(QLabel("Manage Source Directories:"))
        main_layout.addLayout(src_layout_container)
        
        self.setLayout(main_layout)
        
        self.browse_button.setDefault(True)
        self.browse_button.clicked.connect(self.browseFolder)
        self.add_button.setDefault(True)
        self.add_button.clicked.connect(self.addSourceDirectory)
        self.remove_button.setDefault(True)
        self.remove_button.clicked.connect(self.removeSourceDirectory)
        for path_obj in self.source_dir:
            self.combo_box.addItem(str(path_obj))

    def browseFolder(self):
        folder_selected = QFileDialog.getExistingDirectory(self, "Select Directory")
        if folder_selected:
            self.src_path_input.setText(folder_selected)
            
    def addSourceDirectory(self):
        raw_str = self.src_path_input.text().strip()
        
        if not raw_str:
            return
        
        path_obj = Path(raw_str)
        
        if not path_obj.is_dir():
            QMessageBox.warning(self, "Invalid Path", "This folder doesn't exist!")
            return     
        if path_obj in self.source_dir:
            QMessageBox.warning(self, "Duplicate Entry", f"The path is already in the list.")
            self.src_path_input.clear()
            return
        
        self.source_dir.append(path_obj)
        self.combo_box.addItem(str(path_obj))
        self.saveData()
        self.src_path_input.clear()
    
    def removeSourceDirectory(self):
        index = self.combo_box.currentIndex()
        if index != -1:
            self.source_dir.pop(index)
            self.combo_box.removeItem(index)
            self.saveData()

app = QApplication([])
app.setStyle("windows")
window = MainWindow()
window.show()
app.exec()