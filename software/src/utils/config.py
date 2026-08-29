from dataclasses import dataclass
from typing import Optional

@dataclass
class AppConfig:
    log_to_file: bool
    serial_port: Optional[str]
    baudrate: int
    udp_listen_port: Optional[int]
    udp_target_ip: str
    udp_target_port: int