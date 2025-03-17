# getting connection info from config.py and connecting with socket
import socket
import threading
from messageHandler import handle_client  # Assuming handle_client is defined elsewhere
from config import get_port  # config is a file, get_port is the func we will use or can do * for getting all func
from userStorage import UserStorage
from userManager import UserManager



def start_server(host, port):
    # initialize storage and user manager
    user_storage = UserStorage()
    user_manager = UserManager(user_storage)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    s.listen(5)
    print(f"Server listening on {host}:{port}")

    while True:
        conn, addr = s.accept()
        print(f"Connection from {addr}\n")

        # new thread to handle the client connection
        client_thread = threading.Thread(target=handle_client, args=(conn, user_storage, user_manager))
        client_thread.start()
        print("server is ready")


def main():
    host = "127.0.0.1" 
    port = get_port('myport.info')

    start_server(host, port)


if __name__ == "__main__":
    main()
