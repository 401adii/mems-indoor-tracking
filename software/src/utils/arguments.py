import argparse
from .config import AppConfig

def parse_startup_args() -> AppConfig:
    parser = argparse.ArgumentParser(
        description="mems-indoor-tracker application",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    parser.add_argument(
        "--log",
        action="store_true",
        help="Enable logging to file"
    )

    parser.add_argument(
        "--serial-port",
        type=str,
        default=None,
        help="Serial port (e.g. COM3 or /dev/ttyUSB0)"
    )

    parser.add_argument(
        "--baudrate",
        type=int,
        default=115200,
        help="Serial baudrate (e.g. 115200)"
    )

    parser.add_argument(
        '--udp-port',
        type=int,
        default=None,
        help="UDP Listening port"
    )

    args = parser.parse_args()

    return AppConfig(
        log_to_file=args.log,
        serial_port=args.serial_port,
        baudrate=args.baudrate,
        udp_port=args.udp_port
    )