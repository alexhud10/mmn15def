# message_handler.py
# handle clients messages in the high level
# works in parallel with protocolUtils that works on network layer

import uuid
from protocolUtils import *

# decode packet with header and pay load
# header values always the same
# payload is different for each request

'''
==============================
process for each request (registration, users list)
=============================
'''


# request 600 for registration
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
        return False, "Invalid payload"

    name_length = payload[0]
    if len(payload) < 1 + name_length + 1:
        print("Error: Payload missing username or public key length.")
        return False, "Invalid payload"

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
        return False, "Username already exists"
    else:
        user_id = uuid.uuid4().hex[:16]
        user_data = {
            'user_id': user_id,
            'username': username,
            'public_key': public_key if public_key else None,
        }
        user_storage.save_user_data(user_data)
        print(f"User '{username}' registered with ID {user_id}.")
        return True, user_id


# function for request 601 to get users list
def get_users(user_storage, user_id):
    """
    Retrieves all users from user_storage, excluding the requester,
    """
    all_users = user_storage.load_user_data()  # user list contain user dict info
    users_list = []
    for user in all_users:
        print(user)
        if user.get("user_id") != user_id:
            users_list.append({
                "user_id": user.get("user_id", ""),
                "username": user.get("username", "")
            })
    return users_list


'''
==============================
handling requests
=============================
'''


# generate payload for each response code (2xxx)
def build_response(version, code, data=None):
    """
    Given a response code and optional data, build the appropriate payload
    and then create the full response packet (header + payload).
    """
    if code == 2100:
        # Registration success
        # 'data' is expected to be the client_id (string or bytes)
        payload = build_payload_registration_success(data)
    elif code == 2101:
        payload = build_users_payload(data)
    else:
        # Default or unknown code
        # Could just return an empty payload or handle other codes
        payload = b''

    return create_response_packet(version, code, payload)  # creates header and payload packet


def send_response(conn, response_packet):
    """
    Sends the complete response packet to the client over the connection.

    Parameters:
      - conn: a socket object (e.g., from the socket module) already connected to the client.
      - response_packet: a bytes object that represents the full response (header + payload).
    """
    try:
        conn.sendall(response_packet)
        print("Response successfully sent to client.")
    except Exception as e:
        print("Error sending response:", e)


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


def process_request(header, payload, conn, user_storage, user_manager):
    """
    Dispatches processing based on the request code in the header.
    """
    request_code = header.get("request_code")
    if request_code == 600:
        success, response_data = process_registration(payload, conn, user_storage, user_manager)
        if success:
            # Registration success; use code 2100 and data is client_id.
            response_packet = build_response(1, 2100, response_data)
            print("size of data sent: " + str(len(response_packet)))
            send_response(conn, response_packet)
    elif request_code == 601:
        user_id = header.get("client_id", "").strip()
        user_id = bytes.fromhex(user_id).decode('ascii')
        print('server getting user id for user: ' + user_id)
        response_data = get_users(user_storage, user_id)
        response_packet = build_response(1, 2101, response_data)  # build header and payload to binary, generate packet
        print("size of data sent: " + str(len(response_packet)))
        send_response(conn, response_packet)
    else:
        print(f"Unknown request code: {request_code}")
