import re
from PySide6.QtCore import QObject, Signal

from src.utils.logger import get_logger

logger = get_logger("DataBridge")

class DataBridge(QObject):
    sensor_update = Signal(str, str)

    def __init__(self):
        super().__init__()

    def on_data_received(self, data: str):
        match = re.search(r'x:\s*([-\d.]+)\s*y:\s*([-\d.]+)', data)
        if match:
            self.sensor_update.emit(match.group(1), match.group(2))
            logger.info(f"x: {match.group(1)} y: {match.group(2)}")
        else:
            logger.info(f"{data}")