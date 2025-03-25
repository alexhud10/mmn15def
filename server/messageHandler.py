# message_handler.py
# handle clients messages in the high level
# works in parallel with protocolUtils that works on network layer

from protocolUtils import *
from message_storage import *


'''
===================================
processes 
===================================
'''


# function for request 601 to get users list
def get_users(user_storage, user_id):
    """
    retrieves all users from user_storage, excluding the requester
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
    given a response code and optional data, build the appropriate payload
    and then create the full response packet (header + payload).
    """
    if code == 2100:
        payload = build_payload_registration_success(data)
    elif code == 2101:
        payload = build_users_payload(data)
    elif code == 2103:
        payload = build_message_payload(data)
    elif code == 2104:
        payload = build_pull_messages_payload(data)
    elif code == 9000:
        payload = b''
    else:
        payload = b''

    return create_response_packet(version, code, payload)  # creates header and payload packet


def send_response(conn, response_packet):
    """
    sends the complete response packet to the client over the connection.

    parameters:
      - conn: a socket object (e.g., from the socket module) already connected to the client.
      - response_packet: a bytes object that represents the full response (header + payload).
    """
    try:
        header_info = struct.unpack("!B H I", response_packet[:7])
        print(f"[DEBUG] Server sending header: version={header_info[0]}, code={header_info[1]}, payload_size={header_info[2]}")
        conn.sendall(response_packet)
        print("Response successfully sent to client.")
    except Exception as e:
        print("Error sending response:", e)


# receiving messages from client
def handle_client(conn, user_storage, user_manager):
    try:
        # receive data from the client
        data = conn.recv(1024)
        # if no data is received, return
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
    dispatches processing based on the request code in the header.
    """
    request_code = header.get("request_code")
    if request_code == 600:  # registration
        success, response_data = process_registration(payload, user_storage, user_manager)
        if success:
            # Registration success; use code 2100 and data is client_id.
            response_packet = build_response(1, 2100, response_data)  # response_data = get user id
            print("size of data sent: " + str(len(response_packet)))
            send_response(conn, response_packet)
    elif request_code == 601:  # get users list
        user_id = header.get("client_id", "").strip()
        user_id = bytes.fromhex(user_id).decode('ascii')
        print('server getting user id for user: ' + user_id)
        response_data = get_users(user_storage, user_id)  # user list
        response_packet = build_response(1, 2101, response_data)  # build header and payload to binary, generate packet
        print("size of data sent: " + str(len(response_packet)))
        send_response(conn, response_packet)
    elif request_code == 603:  # send message
        user_id = header.get("client_id", "").strip()
        user_id = bytes.fromhex(user_id).decode('ascii')
        recipient_id, message_type, content_size, message_content = process_message(user_id, payload)
        # recipient validation:
        recipient_user = user_storage.get_user_by_id(recipient_id)
        if not recipient_user:
            print(f"Recipient ID {recipient_id} not found.")
            response_packet = build_response(1, 2106, b'User does not exist')
            send_response(conn, response_packet)
            return

        if recipient_user and not user_storage.username_exists(recipient_user['username']):
            print(f"Recipient username {recipient_user['username']} not found.")
            response_packet = build_response(1, 2106, b'User does not exist')
            send_response(conn, response_packet)
            return

        response_data = (recipient_id, message_type, content_size, message_content)
        response_packet = build_response(1, 2103, response_data)
        send_response(conn, response_packet)
        save_to_message_storage(user_id, payload)

    elif request_code == 604:  # get all waiting messages
        recipient_id = header.get("client_id", "").strip()
        recipient_id = bytes.fromhex(recipient_id).decode('ascii')
        print(f"Received message fetch request from: {recipient_id}")
        print("DEBUG: Current MESSAGE_STORAGE:", MESSAGE_STORAGE)  # -------------------
        messages = get_messages_for_recipient(recipient_id)
        print(f"DEBUG: Messages for {recipient_id}: {messages}")  # -------------------
        if not messages:
            print(f"No messages for {recipient_id}")
            # Still send an empty 2104 payload
            response_packet = build_response(1, 2104, b'')
            send_response(conn, response_packet)
            return
        response_packet = build_response(1, 2104, messages)
        send_response(conn, response_packet)
        # clear the messages after sending
        MESSAGE_STORAGE.pop(recipient_id, None)

    else:
        print(f"Unknown request code: {request_code}")
        response_packet = build_response(1, 9000)
        send_response(conn, response_packet)


