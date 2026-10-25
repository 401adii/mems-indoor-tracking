from PySide6.QtWidgets import QMainWindow, QLabel, QVBoxLayout, QWidget
from src.communication.base_interface import BaseCommunication

class MainWindow(QMainWindow):
    def __init__(self, interface: BaseCommunication):
        super().__init__()
        self.interface = interface
        self.setWindowTitle("MEMS Indoor Tracker")
        self.resize(800, 600)

        # Placeholder UI
        central_widget = QWidget()
        layout = QVBoxLayout(central_widget)
        
        info_label = QLabel("Connection Established. Waiting for GUI implementation...")
        layout.addWidget(info_label)
        
        self.setCentralWidget(central_widget)

    def closeEvent(self, event):
        if self.interface:
            self.interface.disconnect()
        event.accept()