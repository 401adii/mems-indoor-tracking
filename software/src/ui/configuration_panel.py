from PySide6.QtWidgets import (QWidget, QVBoxLayout, QGroupBox, QRadioButton, 
                               QComboBox, QPushButton, QHBoxLayout)

class ConfigurationPanel(QWidget):
    def __init__(self, controller):
        super().__init__()
        self.controller = controller
        self.setFixedWidth(250)
        self._setup_ui()
        
    def _setup_ui(self):
        layout = QVBoxLayout(self)
        
        # Sensor Selection
        sensor_group = QGroupBox("Sensor Selection")
        sensor_layout = QVBoxLayout()
        
        self.radio_lsm = QRadioButton("LSM6DSOX")
        self.radio_bno = QRadioButton("BNO085")
        self.radio_lsm.setChecked(True)
        
        sensor_layout.addWidget(self.radio_lsm)
        sensor_layout.addWidget(self.radio_bno)
        sensor_group.setLayout(sensor_layout)
        layout.addWidget(sensor_group)
        
        # Method Selection 
        method_group = QGroupBox("Method")
        method_layout = QVBoxLayout()
        
        self.combo_methods = QComboBox()
        method_layout.addWidget(self.combo_methods)
        method_group.setLayout(method_layout)
        layout.addWidget(method_group)
        
        # Control Buttons
        btn_layout = QHBoxLayout()
        self.btn_start = QPushButton("Start")
        self.btn_stop = QPushButton("Stop")
        
        btn_layout.addWidget(self.btn_start)
        btn_layout.addWidget(self.btn_stop)
        layout.addLayout(btn_layout)
        
        layout.addStretch() 
        
        # Signals / Slots Connections
        self.radio_lsm.toggled.connect(self._update_methods)
        
        self.combo_methods.activated.connect(self._on_method_selected)
        
        self.btn_start.clicked.connect(self.controller.start)
        self.btn_stop.clicked.connect(self.controller.stop)
        
        self._update_methods()

    def _update_methods(self):
        self.combo_methods.blockSignals(True)
        self.combo_methods.clear()
        
        self.combo_methods.addItem("--- Select Method ---")
        
        if self.radio_lsm.isChecked():
            self.combo_methods.addItems(["LSM6DSOX"])
        else:
            self.combo_methods.addItems(["BNO085"])
            
        self.combo_methods.blockSignals(False)

    def _on_method_selected(self, index: int):
        if index == 0:
            return
            
        task = self.combo_methods.currentText()
        if task:
            self.controller.select_task(task)