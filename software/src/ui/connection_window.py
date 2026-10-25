import serial.tools.list_ports
from PySide6.QtWidgets import (QDialog, QVBoxLayout, QHBoxLayout, QRadioButton, 
                               QGroupBox, QFormLayout, QComboBox, QLineEdit, 
                               QPushButton, QStackedWidget, QMessageBox)
from PySide6.QtCore import Qt

class ConnectionWindow(QDialog):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Device Connection Setup")
        self.setMinimumWidth(400)
        self._user_config = {}

        self._setup_ui()
        self._populate_serial_ports()

    def _setup_ui(self):
        main_layout = QVBoxLayout(self)

        # Mode Selection
        mode_group = QGroupBox("Communication Mode")
        mode_layout = QHBoxLayout()
        self.radio_serial = QRadioButton("Serial (USB)")
        self.radio_udp = QRadioButton("UDP (Wi-Fi)")
        self.radio_serial.setChecked(True)
        mode_layout.addWidget(self.radio_serial)
        mode_layout.addWidget(self.radio_udp)
        mode_group.setLayout(mode_layout)
        main_layout.addWidget(mode_group)

        # Stacked Widget for Forms
        self.stack = QStackedWidget()
        
        # Serial Form 
        self.serial_widget = QGroupBox("Serial Configuration")
        serial_layout = QFormLayout()
        self.combo_ports = QComboBox()
        self.combo_baudrate = QComboBox()
        self.combo_baudrate.addItems(["9600", "115200", "460800", "921600"])
        self.combo_baudrate.setCurrentText("115200")
        
        btn_refresh = QPushButton("Refresh Ports")
        btn_refresh.clicked.connect(self._populate_serial_ports)
        
        serial_layout.addRow("Device Port:", self.combo_ports)
        serial_layout.addRow("", btn_refresh)
        serial_layout.addRow("Baudrate:", self.combo_baudrate)
        self.serial_widget.setLayout(serial_layout)
        self.stack.addWidget(self.serial_widget)

        # UDP Form
        self.udp_widget = QGroupBox("UDP Configuration")
        udp_layout = QFormLayout()
        self.input_target_ip = QLineEdit("192.168.4.1")
        self.input_target_port = QLineEdit("3333")
        self.input_listen_port = QLineEdit("4554")
        
        udp_layout.addRow("Target IP:", self.input_target_ip)
        udp_layout.addRow("Target Port (TX):", self.input_target_port)
        udp_layout.addRow("Listen Port (RX):", self.input_listen_port)
        self.udp_widget.setLayout(udp_layout)
        self.stack.addWidget(self.udp_widget)

        main_layout.addWidget(self.stack)

        # Connect Radio Buttons to Stack
        self.radio_serial.toggled.connect(lambda: self.stack.setCurrentIndex(0))
        self.radio_udp.toggled.connect(lambda: self.stack.setCurrentIndex(1))

        # Connect / Cancel Buttons
        btn_layout = QHBoxLayout()
        self.btn_connect = QPushButton("Connect")
        self.btn_cancel = QPushButton("Cancel")
        self.btn_connect.clicked.connect(self._on_connect_clicked)
        self.btn_cancel.clicked.connect(self.reject)
        
        btn_layout.addStretch()
        btn_layout.addWidget(self.btn_cancel)
        btn_layout.addWidget(self.btn_connect)
        main_layout.addLayout(btn_layout)

    def _populate_serial_ports(self):
        self.combo_ports.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.combo_ports.addItem(port.device)
        
        if not ports:
            self.combo_ports.addItem("No devices found")

    def _on_connect_clicked(self):
        try:
            if self.radio_serial.isChecked():
                port = self.combo_ports.currentText()
                if port == "No devices found" or not port:
                    raise ValueError("A valid Serial Port must be selected.")
                
                self._user_config = {
                    "serial_port": port,
                    "baudrate": int(self.combo_baudrate.currentText()),
                    "udp_listen_port": None,
                    "udp_target_ip": None,
                    "udp_target_port": None
                }
            else:
                self._user_config = {
                    "serial_port": None,
                    "baudrate": None,
                    "udp_listen_port": int(self.input_listen_port.text()),
                    "udp_target_ip": self.input_target_ip.text(),
                    "udp_target_port": int(self.input_target_port.text())
                }
            self.accept()
        except ValueError as e:
            QMessageBox.critical(self, "Input Error", str(e))

    def get_config_updates(self) -> dict:
        return self._user_config