from PySide6.QtWidgets import QVBoxLayout, QLabel, QFrame
from PySide6.QtCore import Qt

class DisplayPanel(QFrame):
    def __init__(self):
        super().__init__()
        self.setFrameShape(QFrame.StyledPanel)
        self._setup_ui()

    def _setup_ui(self):
        layout = QVBoxLayout(self) 
        
        self.label_x = QLabel("X: 0.000")
        self.label_y = QLabel("Y: 0.000")
        
        font = self.label_x.font()
        font.setPointSize(48)
        font.setBold(True)
        self.label_x.setFont(font)
        self.label_y.setFont(font)
        
        self.label_x.setAlignment(Qt.AlignCenter)
        self.label_y.setAlignment(Qt.AlignCenter)

        layout.addStretch()
        layout.addWidget(self.label_x)
        layout.addWidget(self.label_y)
        layout.addStretch()

    def update_display(self, x_val: str, y_val: str):
        """Slot to update the labels safely from the main thread."""
        self.label_x.setText(f"X: {x_val}")
        self.label_y.setText(f"Y: {y_val}")