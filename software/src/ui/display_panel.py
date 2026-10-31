import pyqtgraph as pg
from PySide6.QtWidgets import QVBoxLayout, QFrame
from PySide6.QtCore import Qt

class DisplayPanel(QFrame):
    def __init__(self):
        super().__init__()
        self.setFrameShape(QFrame.StyledPanel)
        
        self.axis_range = 11.0 
        
        self._setup_ui()

    def _setup_ui(self):
        layout = QVBoxLayout(self) 
        
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground('w')
        self.plot_widget.setTitle("Live Trajectory", color="k", size="14pt")
        
        self.plot_widget.showGrid(x=True, y=True, alpha=0.3)
        
        self.plot_widget.setLabel('left', 'Y Position (m)', color='k')
        self.plot_widget.setLabel('bottom', 'X Position (m)', color='k')
        self.plot_widget.setXRange(-self.axis_range, self.axis_range, padding=0)
        self.plot_widget.setYRange(-self.axis_range, self.axis_range, padding=0)
        self.plot_widget.setMouseEnabled(x=False, y=False)

        limit = int(self.axis_range)
        major_ticks = [(float(v), str(v)) for v in range(-limit, limit + 1)]
        
        self.plot_widget.getAxis('left').setTicks([major_ticks, []])
        self.plot_widget.getAxis('bottom').setTicks([major_ticks, []])

        self.point = self.plot_widget.plot([0.0], [0.0], pen=None, symbol='o', symbolBrush='r', symbolSize=15)

        layout.addWidget(self.plot_widget)

    def update_display(self, x_val: str, y_val: str):
        try:
            x = float(x_val)
            y = float(y_val)
            self.point.setData([x], [y])
        except ValueError:
            pass

    def reset_plot(self):
        self.point.setData([0.0], [0.0])