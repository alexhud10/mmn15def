# message_handler.py
# handle clients messages in the high level
# works in parallel with protocolUtils that works on network layer

from protocolUtils import *

# decode packet with header and pay load
# header values always the same
# payload is different for each request

'''
==============================
process for each request (users list)
=============================
'''



'''
===================================
processes 
===================================
'''


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
    elif code == 2103:
        payload = build_message_payload(data)
        save_to_message_storage(data)
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
    user_id = header.get("client_id", "").strip()
    user_id = bytes.fromhex(user_id).decode('ascii')
    if request_code == 600:
        success, response_data = process_registration(payload, user_storage, user_manager)
        if success:
            # Registration success; use code 2100 and data is client_id.
            response_packet = build_response(1, 2100, response_data)
            print("size of data sent: " + str(len(response_packet)))
            send_response(conn, response_packet)
    elif request_code == 601:
        # user_id = header.get("client_id", "").strip()
        # user_id = bytes.fromhex(user_id).decode('ascii')
        print('server getting user id for user: ' + user_id)
        response_data = get_users(user_storage, user_id)
        response_packet = build_response(1, 2101, response_data)  # build header and payload to binary, generate packet
        print("size of data sent: " + str(len(response_packet)))
        send_response(conn, response_packet)
    elif request_code == 603:
        # another if for type message
        response_data = process_message(user_id, payload)
        response_packet = build_response(1, 2103, response_data)
    else:
        print(f"Unknown request code: {request_code}")
