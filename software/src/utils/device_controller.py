from src.communication.base_interface import BaseCommunication
from src.utils.logger import get_logger

logger = get_logger("Controller")

class DeviceController:
    def __init__(self, interface: BaseCommunication):
        self.interface = interface

    def select_task(self, task: str):
        logger.info(f"Selecting task: {task}")
        self.interface.send(f"SELECT {task}")

    def start(self):
        logger.info("Sending START command")
        self.interface.send("START")

    def stop(self):
        logger.info("Sending STOP command")
        self.interface.send("STOP")

    def set_output(self, mode: str):
        logger.info(f"Setting output mode: {mode}")
        self.interface.send(f"OUT {mode}")