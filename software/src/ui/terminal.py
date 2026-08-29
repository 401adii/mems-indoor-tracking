import cmd
from src.communication.base_interface import BaseCommunication
from src.utils.logger import get_logger

logger = get_logger("Terminal")

class Terminal(cmd.Cmd):
    intro = "\nTerminal Started (Type 'help' or '?' to list commands. Type 'exit' to quit)\n"
    prompt = "> "

    def __init__(self, interface: BaseCommunication):
        super().__init__()
        self.interface = interface

    def default(self, line: str):
        if line.strip():
            success = self.interface.send(line.strip())
            if not success:
                logger.warning(f"Failed to send command: {line}")

    def emptyline(self):
        """Prevents repeating the last command when the user just presses Enter."""
        pass

    # REGISTERED DEVICE COMMANDS
    def do_blink(self, arg):
        self.interface.send("BLINK")
    
    # LOCAL COMMANDS
    def do_exit(self, arg):
        """Exit the application."""
        logger.info("Closing terminal...")
        return True 

    def do_quit(self, arg):
        """Alias for exit."""
        return self.do_exit(arg)