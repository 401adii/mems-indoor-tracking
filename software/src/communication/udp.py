import socket
from typing import Callable, Any
from .base_interface import BaseCommunication
from src.utils.logger import get_logger

logger = get_logger("UDPComm")

class UDPInterface(BaseCommunication):
    def __init__(self, local_port: int, target_ip: str, target_port: int, receive_callback: Callable[[str], Any]):
        super().__init__(receive_callback)
        self.local_port = local_port
        self.target_ip = target_ip
        self.target_port = target_port
        self._socket = None

    def connect(self) -> bool:
        try:
            self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self._socket.settimeout(0.5)
            self._socket.bind(("", self.local_port))

            logger.info(f"UDP listening on port {self.local_port}.")
            logger.info(f"UDP send target set to {self.target_ip}:{self.target_port}")

            return True
        except OSError as e:
            logger.error(f"Failed to bind UDP socket on port {self.local_port}: {e}")
            return False

    def disconnect(self) -> None:
        self.stop_listening()
        if self._socket:
            self._socket.close()

    def send(self, data: str) -> bool:
        if not self._socket:
            return False

        try:
            self._socket.sendto(f"{data}\n".encode('utf-8'), (self.target_ip, self.target_port))
            logger.info(f"Sent: {data}")
            return True
        except OSError as e:
            logger.error(f"Send Error: {e}")
            return False

    def _receive_loop(self) -> None:
        while self._is_running.is_set():
            if self._socket:
                try:
                    raw_data, addr = self._socket.recvfrom(4096)
                    if raw_data:
                        decoded_data = raw_data.decode('utf-8', errors='ignore').strip()
                        if decoded_data:
                            self._receive_callback(decoded_data)
                except socket.timeout:
                    pass
                except OSError as e:
                    logger.error(f"UDP Receive Error: {e}")