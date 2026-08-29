from src.utils.arguments import parse_startup_args
from src.utils.logger import init_logging, get_logger

def main():
    config = parse_startup_args()

    init_logging(log_to_file=config.log_to_file)
    logger = get_logger("Main")

    logger.info("App starting")
    if config.serial_port:
        logger.info(f"Starting serial communication: {config.serial_port} at {config.baudrate} baud")

if __name__== "__main__":
    main()