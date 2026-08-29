import threading
from abc import ABC, abstractmethod
from typing import Callable, Any

class BaseCommunication(ABC):
    def __init__(self, receive_callback: Callable[[str], Any]):
        self._receive_callback = receive_callback
        self._is_running = threading.Event()
        self._listen_thread = None

    @abstractmethod
    def connect(self) -> bool:
        pass

    @abstractmethod
    def disconnect(self) -> None:
        pass

    @abstractmethod
    def send(self, data: str) -> bool:
        pass

    def start_listening(self) -> None:
        if not self._is_running.is_set():
            self._is_running.set()
            self._listen_thread = threading.Thread(
                target=self._receive_loop,
                daemon=True
            )
            self._listen_thread.start()

    def stop_listening(self) -> None:
        self._is_running.clear()
        if self._listen_thread and self._listen_thread.is_alive():
            self._listen_thread.join(timeout=2.0)

    @abstractmethod
    def _receive_loop(self) -> None:
        pass