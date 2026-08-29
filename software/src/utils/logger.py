import logging
import sys
from pathlib import Path
from datetime import datetime

APP_LOGGER_NAME = "App"

def init_logging(log_to_file) -> logging.Logger:
    logger = logging.getLogger(APP_LOGGER_NAME)

    if logger.handlers:
        return

    logger.setLevel(logging.DEBUG)

    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO)
    console_format = logging.Formatter('[%(levelname)s] %(name)s: %(message)s')
    console_handler.setFormatter(console_format)
    logger.addHandler(console_handler)

    if log_to_file:
        log_dir = Path("logs")
        log_dir.mkdir(parents=True, exist_ok=True)

        timestamp = datetime.now().strftime("%Y-%m_%d_%H_%M_%S")
        log_file = log_dir / f"{timestamp}.log"

        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(logging.DEBUG)
        file_format = logging.Formatter('%(levelname)-6s | %(name)s:%(lineno)d | %(message)s')
        file_handler.setFormatter(file_format)
        logger.addHandler(file_handler)

def get_logger(module_name: str) -> logging.Logger:
    return logging.getLogger(f"{APP_LOGGER_NAME}.{module_name}")