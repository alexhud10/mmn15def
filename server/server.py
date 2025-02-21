
import socket
import threading
from message_handler import handle_client  # Assuming handle_client is defined elsewhere
from config import get_port # confic is a file, get_port is the func we will use or can do * for getting all func

def start_server(host, port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen(5)  

    print(f"Server started on {host}:{port}")

    while True:
        # Accept a client connection
        conn, addr = s.accept()
        print(f"Connection from {addr}")

        # Create a new thread to handle the client connection
        client_thread = threading.Thread(target=handle_client, args=(c,))
        client_thread.start()

def main():
    host = "127.0.0.1"  # Localhost
    port = 8080          # Default port

    # Load configurations (like port) if needed
    # For example, you might want to use `config.py` to read from a file:
    # port = config.get_port_from_file('myport.info')

    start_server(host, port)

if __name__ == "__main__":
    main()
