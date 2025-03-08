# message_handler.py
# handle clients messages
import socket
import struct
from userStorage import UserStorage
from userManager import UserManager
import uuid

# decode packet with header and pay load
# header values always the same
# payload is different for each request
def decode_packet(data):
    """
    Decodes a packet that contains:
      - 16 bytes: client_id
      - 1 byte: version
      - 2 bytes: request code
      - 4 bytes: payload size
    Followed by the payload.

    Returns a tuple (header, payload) where header is a dictionary.
    """
    HEADER_FORMAT = "!16s B H I"  # network order: 16-byte string, 1-byte, 2-byte, 4-byte
    HEADER_SIZE = struct.calcsize(HEADER_FORMAT)  # Should be 16 + 1 + 2 + 4 = 23 bytes

    if len(data) < HEADER_SIZE:
        print("Error: Packet too short to contain valid header.")
        return None, None

    client_id_bytes, version, request_code, payload_size = struct.unpack(HEADER_FORMAT, data[:HEADER_SIZE])
    header = {
        "client_id": client_id_bytes.hex(),
        "version": version,
        "request_code": request_code,
        "payload_size": payload_size,
    }
    payload = data[HEADER_SIZE:HEADER_SIZE + payload_size]
    return header, payload

# decode for each payload


# Helper: create a response packet.
# Protocol for the response packet:
#   1 byte: status (1 for success, 0 for error)
#   1 byte: length of the following string (user ID or error message)
#   N bytes: the actual string (UTF-8 encoded)
def create_response_packet(message, success=True):
    """
    Creates a response packet containing:
      - 1 byte: status (1 for success, 0 for error)
      - 1 byte: message length
      - N bytes: UTF-8 encoded message
    """
    msg_bytes = message.encode('utf-8')
    msg_length = len(msg_bytes)
    status = 1 if success else 0
    # Pack as: status (1 byte), length (1 byte), then the message
    packet = struct.pack("BB", status, msg_length) + msg_bytes
    return packet


def process_registration(payload, conn, user_storage, user_manager):
    """
    Processes a registration request (request code 600).
    Assumes payload format:
      - 1 byte: name_length
      - n bytes: username (UTF-8)
      - 1 byte: public_key_length
      - p bytes: public key (UTF-8, can be empty)
    """
    if len(payload) < 1:
        print("Error: Payload too short for registration.")
        response = create_response_packet("Invalid payload", success=False)
        conn.sendall(response)
        return

    name_length = payload[0]
    if len(payload) < 1 + name_length + 1:
        print("Error: Payload missing username or public key length.")
        response = create_response_packet("Invalid payload", success=False)
        conn.sendall(response)
        return

    username = payload[1:1 + name_length].decode('utf-8')
    pk_index = 1 + name_length
    public_key_length = payload[pk_index]
    if len(payload) < pk_index + 1 + public_key_length:
        public_key = None
    else:
        public_key = payload[pk_index + 1: pk_index + 1 + public_key_length].decode('utf-8')

    print(f"Registration request for username: {username}")

    if user_storage.username_exists(username):
        print(f"Error: Username '{username}' already exists.")
        response = create_response_packet("Username already exists", success=False)
    else:
        user_id = str(uuid.uuid4())
        user_data = {
            'user_id': user_id,
            'username': username,
            'public_key': public_key if public_key else None,
        }
        user_storage.save_user_data(user_data)
        print(f"User '{username}' registered with ID {user_id}.")
        response = create_response_packet(user_id, success=True)
    conn.sendall(response)


def process_request(header, payload, conn, user_storage, user_manager):
    """
    Dispatches processing based on the request code in the header.
    """
    request_code = header.get("request_code")
    if request_code == 600:
        process_registration(payload, conn, user_storage, user_manager)
    else:
        print(f"Unknown request code: {request_code}")
        response = create_response_packet("Unknown request", success=False)
        conn.sendall(response)

# receiving messages from client
def handle_client(conn, user_storage, user_manager):
    try:
        # Receive data from the client
        data = conn.recv(1024)
        # If no data is received, return
        if not data:
            print("no data received \n")
            return

        header, payload = decode_packet(data)
        if header is None:
            print("Error decoding packet.")
            return

        print("Decoded Header:", header)
        process_request(header, payload, conn, user_storage, user_manager)
    except Exception as e:
        print(f"Error handling client: {e} \n")

    finally:
        conn.close()
