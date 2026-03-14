import file_sorter as sort
import json
from functools import partial
from pathlib import Path
from PyQt6.QtWidgets import (QApplication, QHeaderView, QPushButton, QLineEdit, QTableWidgetItem, QMenu, QWidget, QLabel, QSizePolicy,
                             QVBoxLayout, QHBoxLayout, QTableWidget, QComboBox, QMessageBox, QFileDialog, QGroupBox)
from PyQt6.QtGui import QIcon, QAction

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        
        self.config_file = "settings.json"
        self.roots = [] # search roots list. where the program will look for files to move.
        self.target = Path() # a single target directory. where the files will be sorted into.
        self.tuples = [("",[])] # list of (folder_name, extensions) touples. the program will sort files into the folder_name in the targetted folder.
        
        self.runtime_keys = {}
        self.runtime_map = {}
        
        self.labels = [QLabel("Manage Search Roots:"), QLabel("Select Target Path:"), QLabel("Sorting Rules:")]
        self.browse_buttons = [QPushButton("Browse..."), QPushButton("Browse...")]
        self.add_buttons = [QPushButton("Add"), QPushButton("Add"), QPushButton("Move Files")]
        self.remove_buttons = [QPushButton("Remove"), QPushButton("Remove")]
        self.path_inputs = [QLineEdit(), QLineEdit()]
        self.sort_action = QAction("Sort Files")
        self.unsort_action = QAction("Unsort Files")
        
        self.loadData()
        self.initUI()
        
    def loadData(self):
        file_path = Path(self.config_file)
        if file_path.exists():
            with open(file_path, 'r') as f:
                data = json.load(f)
            
            self.roots = [Path(p) for p in data["search_roots"]]
            if data["target"] == "." or data["target"]:
                self.path_inputs[1].setText(data["target"])
                self.target = Path(data["target"])
            self.tuples = [(folder, exts) for folder, exts in data["sorting_rules"]]
        else:
            self.saveData() # create the file if it doesn't exist.
            
        self.runtime_keys = { # flattening tuples for backend operations.
            ext: folder 
            for folder, ext_list in self.tuples 
            for ext in ext_list
        }
        self.runtime_map = sort.build_dest_map(self.target, self.runtime_keys)
    
    def saveData(self):
        roots_to_save = {"search_roots": [str(p) for p in self.roots]}
        tuples_to_save = [(folder, exts) for folder, exts in self.tuples]
        with open(self.config_file, 'w') as f:
            json.dump({**roots_to_save, "target": str(self.target), "sorting_rules": tuples_to_save}, f, indent=4)
        
    def initUI(self):
        self.setWindowTitle("File Organizer Project")
        self.setWindowIcon(QIcon("lain.ico"))
        self.setFixedSize(700, 550) # width, height
        
        main_layout = QVBoxLayout()
        
        main_layout.addLayout(self.rootPathsLayout()) # where you add/remove search roots.
        main_layout.addLayout(self.targetPathsLayout())
        main_layout.addLayout(self.sortingSettingsLayout()) # where you set the sorting rules.
        main_layout.addLayout(self.actionButtonsLayout())
        self.setLayout(main_layout)

        for lbl in self.labels:
            lbl.setStyleSheet("font-weight: bold;")
        for i, btn in enumerate(self.browse_buttons):
            btn.setDefault(True)
            btn.clicked.connect(partial(self.browseFolder, i))
        for btn in self.add_buttons:
            btn.setDefault(True)
        for i, btn in enumerate(self.remove_buttons):
            btn.setDefault(True)
            self.remove_buttons[i].setStyleSheet("background-color: #ffcccc;")
        self.add_buttons[0].clicked.connect(partial(self.addPath, 0))
        self.remove_buttons[0].clicked.connect(self.removeRoot)
        self.path_inputs[1].textChanged.connect(self.updateTargetPath)
        self.add_buttons[1].clicked.connect(self.addTableRow)
        self.remove_buttons[1].clicked.connect(self.removeTableRow)
        self.add_buttons[2].clicked.connect(lambda: sort.to_src_dir(self.target, self.roots, self.runtime_map))
        self.table.itemChanged.connect(self.updateTuples)
        self.sort_action.triggered.connect(lambda: self.handle_action("Sort"))
        self.unsort_action.triggered.connect(lambda: self.handle_action("Unsort"))
        
        for path_obj in self.roots:
            self.combo_box.addItem(str(path_obj))
        
        self.table.setRowCount(len(self.tuples))
        for row, (folder, exts) in enumerate(self.tuples):
            folder_item = QTableWidgetItem(folder)
            exts_item = QTableWidgetItem(", ".join(exts))
            
            self.table.setItem(row, 0, folder_item)
            self.table.setItem(row, 1, exts_item)
    
    def updateTargetPath(self, raw_str):
        if not raw_str.strip():
            return
        path_obj = Path(raw_str)
        if not path_obj.is_dir():
            return
        if path_obj in self.roots:
            return
        
        self.target = path_obj
        self.runtime_map = sort.build_dest_map(self.target, self.runtime_keys)
        self.saveData()

    def browseFolder(self, index):
        folder_selected = QFileDialog.getExistingDirectory(self, "Select Directory")
        if folder_selected:
            self.path_inputs[index].setText(folder_selected)
            
    def addPath(self, index):
        raw_str = self.path_inputs[index].text().strip()
        if not raw_str:
            return
        
        path_obj = Path(raw_str)
        if not path_obj.is_dir():
            QMessageBox.warning(self, "Invalid Path", "This folder doesn't exist!")
            return     
        if path_obj in self.roots:
            QMessageBox.warning(self, "Duplicate Entry", "The path is already in the list.")
            self.path_inputs[index].clear()
            return
        if path_obj == self.target:
            QMessageBox.warning(self, "Already Targeted", "The path is already a target.")
            return
        
        self.roots.append(path_obj)
        self.combo_box.addItem(str(path_obj))
        self.saveData()
        self.path_inputs[index].clear()
    
    def removeRoot(self):
        index = self.combo_box.currentIndex()
        if index != -1:
            self.roots.pop(index)
            self.combo_box.removeItem(index)
            self.saveData()
            
    def updateTuples(self, item):
        new_tuples = []
        for row in range(self.table.rowCount()):
            folder_item = self.table.item(row, 0)
            folder_name = folder_item.text().strip() if folder_item else ""
            exts_item = self.table.item(row, 1)
            if exts_item:
                raw_exts = exts_item.text().split(",")
                exts_list = [e.strip() for e in raw_exts if e.strip()]
            else:
                exts_list = []
            new_tuples.append((folder_name, exts_list))
        
        self.tuples = new_tuples
        self.runtime_keys = {
            ext: folder
            for folder, ext_list in self.tuples
            for ext in ext_list
        }
        self.runtime_map = sort.build_dest_map(self.target, self.runtime_keys)
        self.saveData()
        
    def addTableRow(self):
        row_pos = self.table.rowCount()
        self.table.insertRow(row_pos)
        self.table.setItem(row_pos, 0, QTableWidgetItem(""))
        self.table.setItem(row_pos, 1, QTableWidgetItem(""))
        self.updateTuples(None)
        
    def removeTableRow(self):
        current_row = self.table.currentRow()
        if current_row != -1:
            self.table.removeRow(current_row)
            self.updateTuples(None)
        else:
            QMessageBox.information(self, "Selection Required", "Please click a row to delete.")

    def rootPathsLayout(self):
        root_layout_add = QHBoxLayout() # source directory add layout
        
        self.path_inputs[0].setPlaceholderText("Add a search root...")
        
        root_layout_add.addWidget(self.path_inputs[0])
        root_layout_add.addWidget(self.add_buttons[0])
        root_layout_add.addWidget(self.browse_buttons[0])
        
        root_layout_remove = QHBoxLayout() # source directory remove layout
        self.combo_box = QComboBox()
        self.combo_box.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        
        root_layout_remove.addWidget(self.combo_box)
        root_layout_remove.addWidget(self.remove_buttons[0])

        root_layout_container = QVBoxLayout()

        root_layout_container.addWidget(self.labels[0])
        root_layout_container.addLayout(root_layout_add)
        root_layout_container.addLayout(root_layout_remove)
        return root_layout_container

    def targetPathsLayout(self):
        target_layout = QHBoxLayout()
        
        self.path_inputs[1].setPlaceholderText("Add a target path...")
        
        target_layout.addWidget(self.path_inputs[1])
        target_layout.addWidget(self.add_buttons[2])
        target_layout.addWidget(self.browse_buttons[1])
        
        target_layout_container = QVBoxLayout()
        
        target_layout_container.addWidget(self.labels[1])
        target_layout_container.addLayout(target_layout)
        
        return target_layout_container

    def sortingSettingsLayout(self):
        sort_rules_manager = QHBoxLayout()
        
        sort_rules_manager.addWidget(self.add_buttons[1])
        sort_rules_manager.addWidget(self.remove_buttons[1])
        
        self.table = QTableWidget()
        self.table.setColumnCount(2)
        self.table.setHorizontalHeaderLabels(["Folder Name", "Extensions"])
        
        header = self.table.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.ResizeMode.ResizeToContents)
        header.setSectionResizeMode(1, QHeaderView.ResizeMode.Stretch)

        sort_rules_layout_container = QVBoxLayout()
        sort_rules_layout_container.addWidget(self.labels[2])
        sort_rules_layout_container.addLayout(sort_rules_manager)
        sort_rules_layout_container.addWidget(self.table)
        return sort_rules_layout_container
    
    def actionButtonsLayout(self):
        sort_layout = QHBoxLayout()
        self.sort_group = QGroupBox("Sorting actions")
        self.sort_dropdown = QComboBox()
        self.sort_dropdown.addItems(["By Name", "By Date", "By Size", "By Extension"])
        self.sort_dropdown.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sort_layout.addWidget(self.sort_dropdown)
        
        self.confirm_btn = QPushButton("Choose Action...")
        
        action_menu = QMenu(self)
        action_menu.addActions([self.sort_action, self.unsort_action])
        
        self.confirm_btn.setMenu(action_menu)
        sort_layout.addWidget(self.confirm_btn)
        self.sort_group.setLayout(sort_layout)
        
        actions_panel = QVBoxLayout()
        actions_panel.addWidget(self.sort_group)
        
        return actions_panel
    
app = QApplication([])
app.setStyle("windows")
window = MainWindow()
window.show()
app.exec()