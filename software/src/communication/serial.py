import time
from typing import Callable, Any
from serial import Serial, SerialException
from src.utils.logger import get_logger
from .base_interface import BaseCommunication

logger = get_logger("SerialComm")

class SerialInterface(BaseCommunication):
    def __init__(self, port: str, baudrate: int, receive_callback: Callable[[str], Any]):
        super().__init__(receive_callback)
        self.port = port
        self.baudrate = baudrate
        self._serial = None

    def connect(self) -> bool:
        try:
            self._serial = Serial(self.port, self.baudrate, timeout=0.5)
            return True
        except SerialException as e:
            logger.error(f"Connection Error: {e}")
            return False

    def disconnect(self) -> None:
        self.stop_listening()
        if self._serial and self._serial.is_open:
            self._serial.close()

    def send(self, data: str) -> bool:
        if not self._serial or not self._serial.is_open:
            return False

        try:
            self._serial.write(f"{data}\n".encode('utf-8'))
            logger.info(f"Sent: {data}")
            return True
        except SerialException as e:
            logger.error(f"Send Error: {e}")

    def _receive_loop(self) -> None:
        while self._is_running.is_set():
            if self._serial and self._serial.is_open:
                try:
                    raw_data = self._serial.readline()
                    if raw_data:
                        decoded_data = raw_data.decode('utf-8', errors='ignore').strip()
                        if decoded_data:
                            self._receive_callback(decoded_data)
                except SerialException as e:
                    logger.error(f"Receive Error: {e}")
                    time.sleep(1)
            else:
                time.sleep(0.1)