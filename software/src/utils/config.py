import argparse
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Optional
from src.utils.logger import get_logger

logger = get_logger("Config")

@dataclass
class AppConfig:
    log_to_file: bool
    serial_port: Optional[str]
    baudrate: int
    udp_listen_port: Optional[int]
    udp_target_ip: str
    udp_target_port: int

def load_default_from_file(filepath: str = "config.json") -> dict:
    path = Path(filepath)
    if path.exists():
        try:
            with open(path, 'r') as file:
                return json.load(file)
        except json.JSONDecodeError as e:
            logger.warning(f"Failed to parse {filepath}: {e}")
    return {}

def parse_startup_args() -> AppConfig:
    parser = argparse.ArgumentParser(
        description="mems-indoor-tracker application",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    parser.add_argument('--log', action='store_true', help="Enable detailed logging")
    
    # Serial config
    parser.add_argument('--serial-port', type=str, default=None, help="Default serial port")
    parser.add_argument('--baudrate', type=int, default=None, help="Default serial baudrate")
    
    # UDP config
    parser.add_argument('--udp-listen-port', type=int, default=None, help="Port to listen for incoming data")
    parser.add_argument('--udp-target-ip', type=str, default=None, help="IP address of the target device")
    parser.add_argument('--udp-target-port', type=int, default=None, help="Port of the target device")

    file_defaults = load_default_from_file()

    if file_defaults:
        parser.set_defaults(**file_defaults)

    args = parser.parse_args()
    
    return AppConfig(
        log_to_file=args.log,
        serial_port=args.serial_port,
        baudrate=args.baudrate,
        udp_listen_port=args.udp_listen_port,
        udp_target_ip=args.udp_target_ip,
        udp_target_port=args.udp_target_port
    )