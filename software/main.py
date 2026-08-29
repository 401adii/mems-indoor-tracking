import sys
import time

from src.utils.config import parse_startup_args
from src.utils.logger import init_logging, get_logger

from src.communication.serial import SerialInterface
from src.communication.udp import UDPInterface

def on_data_received(data: str):
    logger = get_logger("Main")
    logger.info(f"Incoming data: {data}")

def main():
    config = parse_startup_args()

    init_logging(log_to_file=config.log_to_file)
    logger = get_logger("Main")
    logger.info("App starting")

    if config.serial_port and config.udp_listen_port:
        logger.error("Only one communication type can be used at a time")
        sys.exit(1)

    interface = None

    if config.serial_port:
        interface = SerialInterface(
            port = config.serial_port,
            baudrate=config.baudrate,
            receive_callback=on_data_received
        )
    elif config.udp_listen_port:
        interface = UDPInterface(
            local_port=config.udp_listen_port,
            target_ip=config.udp_target_ip,
            target_port=config.udp_target_port,
            receive_callback=on_data_received
        )
    else:
        logger.error("No communication interface specified. Use --serial-port od --udp-listen-port.")

    if interface.connect():
        logger.info("Connection established. Starting listener thread.")
        interface.start_listening()
    else:
        logger.error("Failed to establish connection. Exiting")
        sys.exit(1)

    try:
        while True:
            time.sleep(5)
            interface.send("BLINK")
    except KeyboardInterrupt:
        logger.info("Keyboard interrupt. Exiting")
    finally:
        if interface:
            interface.disconnect()
            logger.info("Port disconnected")            

        
if __name__== "__main__":
    main()