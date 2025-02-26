# message_handler.py
# handle clients messages
import socket

def handle_client(conn):
    try:
        # Receive data from the client
        data = conn.recv(1024)

        # If no data is received, return
        if not data:
            print("no data received \n")
            return

        # Print message from client
        print(f"Received message from {conn.getpeername()}: {data.decode('utf-8')} \n")

        # Send a response back to the client
        response = "got your message \n"
        conn.send(response.encode('utf-8'))  # Send the response back to the client

    except Exception as e:
        print(f"Error handling client: {e} \n")

    finally:
        conn.close()
