import sys
from PySide6.QtWidgets import QApplication, QDialog

from src.utils.config import parse_startup_args
from src.utils.logger import init_logging, get_logger
from src.utils.device_controller import DeviceController

from src.communication.serial import SerialInterface
from src.communication.udp import UDPInterface

from src.ui.connection_window import ConnectionWindow
from src.ui.main_window import MainWindow

def on_data_received(data: str):
    logger = get_logger("Main")
    logger.info(f"Incoming data: {data}")

def main():
    config = parse_startup_args()
    init_logging(log_to_file=config.log_to_file)
    logger = get_logger("Main")
    logger.info("App starting")

    app = QApplication(sys.argv)

    if not config.serial_port and not config.udp_listen_port:
        logger.info("No communication configuration found. Prompting user...")
        conn_window = ConnectionWindow()

        if conn_window.exec() == QDialog.Accepted:
            updates = conn_window.get_config_updates()
            config.serial_port = updates["serial_port"]
            config.baudrate = updates["baudrate"]
            config.udp_listen_port = updates["udp_listen_port"]
            config.udp_target_ip = updates["udp_target_ip"]
            config.udp_target_port = updates["udp_target_port"]
        else:
            logger.info("Connection setup cancelled by user. Exiting.")
            sys.exit(0)
    elif config.serial_port and config.udp_listen_port:
        logger.error("Only one communication type can be used at a time.")
        sys.exit(1)

    interface = None
    is_udp = False
    
    if config.serial_port:
        interface = SerialInterface(
            port=config.serial_port,
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
        is_udp = True

    if interface.connect():
        logger.info("Connection established. Starting listener thread.")
        interface.start_listening()
    else:
        logger.error("Failed to establish connection. Exiting.")
        sys.exit(1)

    controller = DeviceController(interface)
    
    if is_udp:
        controller.set_output("UDP")

    main_window = MainWindow(controller)
    main_window.show()

    exit_code = app.exec()
    
    logger.info("Application closed.")
    sys.exit(exit_code)

if __name__== "__main__":
    main()