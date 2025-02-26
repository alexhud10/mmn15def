# open file with connection info

def get_port(file_path):
    try:
        with open(file_path, 'r') as file:
            port = file.read().strip()
            port = int(port)

            return port
    except (FileNotFoundError, ValueError) as e:
        print(f"Error: {e}. Returning default port 1234.")
        return 1234


