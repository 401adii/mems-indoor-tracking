import argparse
from .config import AppConfig

def parse_startup_args() -> AppConfig:
    parser = argparse.ArgumentParser(
        description="mems-indoor-tracker application",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    parser.add_argument('--log', action='store_true', help="Enable detailed logging")
    
    # Serial config
    parser.add_argument('--serial-port', type=str, default=None, help="Default serial port")
    parser.add_argument('--baudrate', type=int, default=115200, help="Default serial baudrate")
    
    # UDP config
    parser.add_argument('--udp-listen-port', type=int, default=None, help="Port to listen for incoming data")
    parser.add_argument('--udp-target-ip', type=str, default="192.168.4.1", help="IP address of the target device")
    parser.add_argument('--udp-target-port', type=int, default=3333, help="Port of the target device")

    args = parser.parse_args()
    
    return AppConfig(
        log_to_file=args.log,
        serial_port=args.serial_port,
        baudrate=args.baudrate,
        udp_listen_port=args.udp_listen_port,
        udp_target_ip=args.udp_target_ip,
        udp_target_port=args.udp_target_port
    )