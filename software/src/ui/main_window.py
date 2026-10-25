from PySide6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QLabel, QFrame
from src.utils.device_controller import DeviceController
from src.ui.configuration_panel import ConfigurationPanel

class MainWindow(QMainWindow):
    def __init__(self, controller: DeviceController):
        super().__init__()
        self.controller = controller
        self.setWindowTitle("MEMS Indoor Tracker")
        self.resize(1000, 600)

        # Central Widget & Main Layout
        central_widget = QWidget()
        main_layout = QHBoxLayout(central_widget)

        # 1. Main View Plane (Placeholder)
        self.view_plane = QFrame()
        self.view_plane.setFrameShape(QFrame.StyledPanel)
        view_layout = QHBoxLayout(self.view_plane)
        placeholder_label = QLabel("Visualization Plane")
        view_layout.addWidget(placeholder_label)

        # Configuration Plane
        self.config_panel = ConfigurationPanel(self.controller)

        # Add to layout and set proportions
        main_layout.addWidget(self.view_plane, stretch=3)
        main_layout.addWidget(self.config_panel, stretch=1)

        self.setCentralWidget(central_widget)

    def closeEvent(self, event):
        if self.controller and self.controller.interface:
            self.controller.interface.disconnect()
        event.accept()