# Filename: argument_parser.py

import sys

class ArgumentParser:
    def __init__(self):
        self.help = False
        self.run_mode = False
        self.ki = False
        self.server_mode = False
        self.program_call = None  # New attribute for the -p argument
        self.parse_arguments()

    def parse_arguments(self):
        if len(sys.argv) > 1:
            if '-h' in sys.argv or '--help' in sys.argv:
                self.help = True
            if '-r' in sys.argv:
                self.run_mode = True
                program_index = sys.argv.index('-r') + 1
                if program_index < len(sys.argv):
                    self.program_to_run = sys.argv[program_index]
                else:
                    print("Kein Programm angegeben. Verwenden Sie das Format 'python3 main.py -r <program>'")
                    sys.exit(1)
            if '-s' in sys.argv:
                self.server_mode = True
            if '-ki' in sys.argv:
                self.ki = True
            if '-p' in sys.argv:  # New condition for the -p argument
                program_call_index = sys.argv.index('-p') + 1
                if program_call_index < len(sys.argv):
                    self.program_call = sys.argv[program_call_index]
                else:
                    print("Kein Programmaufruf angegeben. Verwenden Sie das Format 'python3 main.py -p python3'")
                    sys.exit(1)

    def print_help(self):
        help_message = """
Verwendung: python script.py [OPTION]

Optionen:
  -h, --help         Zeigt diese Hilfenachricht an
  -r PROGRAMM        Startet den Run-Server und wartet auf Befehle, um das angegebene Programm auszuführen
  -s                 Startet den Server
  -ki                Führt die OpenAI-Integration aus und generiert Code basierend auf einer Beschreibung
  -p PROGRAMMAUFRUF  Führt den angegebenen Programmaufruf aus

Ohne Parameter startet das Skript im interaktiven Modus und erwartet Eingaben im folgenden Format:
  DATEINAME

Beispiele:
  python script.py -h
  python script.py -r /mnt/c/tmp/test.py
  python script.py -s
  python script.py -ki
  python script.py -p "ls -la"
"""
        print(help_message)

# Example usage
if __name__ == "__main__":
    parser = ArgumentParser()
    if parser.help:
        parser.print_help()
    # You can add other logic here to handle the parsed arguments as needed
