from PySide6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QLabel, QFrame
from src.core.device_controller import DeviceController
from src.ui.configuration_panel import ConfigurationPanel
from src.ui.display_panel import DisplayPanel

class MainWindow(QMainWindow):
    def __init__(self, controller: DeviceController):
        super().__init__()
        self.controller = controller
        self.setWindowTitle("MEMS Indoor Tracker")
        self.resize(1000, 600)

        # Central Widget & Main Layout
        central_widget = QWidget()
        main_layout = QHBoxLayout(central_widget)

        # Main View Plane
        self.display_panel = DisplayPanel()
        
        # Configuration Plane
        self.config_panel = ConfigurationPanel(self.controller)

        # Add to layout and set proportions
        main_layout.addWidget(self.display_panel, stretch=3)
        main_layout.addWidget(self.config_panel, stretch=1)

        self.setCentralWidget(central_widget)

    def closeEvent(self, event):
        if self.controller and self.controller.interface:
            self.controller.interface.disconnect()
        event.accept()