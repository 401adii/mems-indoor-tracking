import time
import math
import random
from typing import Callable, Any
from src.communication.base_interface import BaseCommunication
from src.utils.logger import get_logger

logger = get_logger("MockComm")

class MockInterface(BaseCommunication):
    def __init__(self, receive_callback: Callable[[str], Any]):
        super().__init__(receive_callback)
        self.is_streaming = False

    def connect(self) -> bool:
        logger.info("Mock hardware interface connected.")
        return True

    def disconnect(self) -> None:
        self.stop_listening()
        self.is_streaming = False
        logger.info("Mock hardware interface disconnected.")

    def send(self, data: str) -> bool:
        logger.info(f"Mock TX: {data}")
        clean_cmd = data.strip()

        if clean_cmd == "START":
            self.is_streaming = True
        elif clean_cmd == "STOP":
            self.is_streaming = False
        elif clean_cmd == "RESTART":
            self.is_streaming = False
            logger.info("Mock device restart simulation triggered.")

        return True

    def _receive_loop(self) -> None:
        theta = 0.0
        tick = 1000000
        step_dt = 0.02    
        angular_speed = 0.1675

        while self._is_running.is_set():
            if self.is_streaming:
                tick += int(step_dt * 1000)

                circle_index = int(theta // (2 * math.pi))
                radius = 0.5 * (circle_index + 1)

                x = radius * math.cos(theta)
                y = radius * math.sin(theta)
                z = 0.000  # Planar displacement

                sample = f"I ({tick}) APP_TASKS: x: {x:.3f} y: {y:.3f} z: {z:.3f}"
                self._receive_callback(sample)

                theta += angular_speed
                time.sleep(step_dt)
            else:
                theta = 0.0
                time.sleep(0.05)