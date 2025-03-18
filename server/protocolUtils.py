# binary protocol implementation

import struct
import random
from userManager import *
'''
===================================
decode packet arrived from client
===================================
'''
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
    header_format = "!16s B H I"  # network order: 16-byte string, 1-byte, 2-byte, 4-byte
    header_size = struct.calcsize(header_format)  # Should be 16 + 1 + 2 + 4 = 23 bytes

    if len(data) < header_size:
        print("Error: Packet too short to contain valid header.")
        return None, None

    client_id_bytes, version, request_code, payload_size = struct.unpack(header_format, data[:header_size])
    header = {
        "client_id": client_id_bytes.hex(),
        "version": version,
        "request_code": request_code,
        "payload_size": payload_size,
    }
    payload = data[header_size:header_size + payload_size]
    return header, payload
'''
===================================
processes for each request - includes payload decoding
===================================
'''
# request 600 for registration
def process_registration(payload, user_storage, user_manager):
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
        return False, "Username already exists"
    else:
        user_manager.register_user(username, public_key)

def process_message(user_id, payload):

    """
    Processes a messaging request payload.

    Expected payload layout:
      - 16 bytes: Recipient ID (ASCII)
      - 1 byte: Message Type (expected to be 3 for text)
      - 4 bytes: Content size (big-endian)
      - n bytes: Message Content (UTF-8 encoded)
    """

    # Check that the payload is at least 21 bytes (16+1+4)
    if len(payload) < 21:
        raise ValueError("Payload too short for processing message.")

    # Extract recipient ID (first 16 bytes) and convert to ASCII, stripping trailing nulls.
    recipient_id_bytes = payload[:16]
    recipient_id = recipient_id_bytes.decode('ascii').rstrip('\0')

    # Extract the message type (1 byte at index 16)
    message_type = payload[16]

    # Extract content size from the next 4 bytes (big-endian)
    content_size = int.from_bytes(payload[17:21], byteorder='big')

    # Ensure the payload contains the expected number of bytes for the message content.
    if len(payload) < 21 + content_size:
        raise ValueError("Incomplete payload: expected {} bytes of content, but got {}".format(21+content_size, len(payload)))

    # Extract the message content (starting at byte 21)
    message_content_bytes = payload[21:21+content_size]
    message_content = message_content_bytes.decode('utf-8')

    # Return the relevant values.
    return recipient_id, message_type, content_size, message_content


'''
===================================
response
===================================
'''

# header to binary
def create_response_header(version, code, payload_size):
    """
    Build the 7-byte header in network byte order:
      - version (1 byte)
      - code (2 bytes)
      - payload_size (4 bytes)
    """
    # "!B H I" means:
    #  - ! : network (big-endian) byte order
    #  - B : 1-byte unsigned char
    #  - H : 2-byte unsigned short
    #  - I : 4-byte unsigned int
    return struct.pack("!B H I", version, code, payload_size)


# header and payload into packet
def create_response_packet(version, code, payload):
    """
    Create a full response packet = header + payload.
    - version (1 byte)
    - code (2 bytes)
    - payload_size (4 bytes)
    - payload (variable length)
    """
    payload_size = len(payload)
    header = create_response_header(version, code, payload_size)
    return header + payload

'''
==============================
generate payload for each request
=============================
'''


# payload builder for registration (code 2100)
def build_payload_registration_success(client_id):
    """
    The spec says that on registration success (code 2100),
    the payload contains the 16-byte client ID.
    If 'client_id' is already a 16-byte value, just return it.
    Otherwise, ensure it's exactly 16 bytes (e.g., pad or truncate).
    """
    # If you store client_id as a 16-byte array, just return it:
    if isinstance(client_id, bytes) and len(client_id) == 16:
        return client_id

    # Otherwise, if it's a string and you need to encode:
    client_id_bytes = client_id.encode('utf-8')  # or your chosen encoding
    # Ensure exactly 16 bytes (pad or truncate as needed):
    client_id_bytes = client_id_bytes[:16].ljust(16, b'\0')
    return client_id_bytes


def build_users_payload(users_list):
    """
    given a list of user dictionaries (with 'user_id' and 'username'),
    builds a binary payload where each user record is:
      - 16 bytes for the user_id (ASCII)
      - 255 bytes for the username (ASCII)

    returns a bytes object representing the payload.
    """
    payload_bytes = bytearray()

    for user in users_list:
        # Process the user ID (16 bytes)
        uid = user.get("user_id", "")
        uid_bytes = uid.encode("ascii", errors="ignore")[:16]
        uid_bytes = uid_bytes.ljust(16, b'\0')

        # Process the username (255 bytes)
        uname = user.get("username", "")
        # We use 254 bytes for characters then add a null terminator (or directly slice 255 if you include it)
        uname_bytes = uname.encode("ascii", errors="ignore")[:254]
        uname_bytes += b'\0'
        uname_bytes = uname_bytes.ljust(255, b'\0')

        # Append the two fields to the payload
        payload_bytes.extend(uid_bytes)
        payload_bytes.extend(uname_bytes)

    return bytes(payload_bytes)

#message id
def generate_message_id():
    return random.getrandbits(32)


# data are decoded recipient_id, message_type, content_size, message_content from process_message
def build_message_payload(data):
    """
    Given a tuple (recipient_id, message_type, content_size, message_content),
    build a binary payload with the following layout:
      - 16 bytes: recipient_id (ASCII, padded/truncated)
      - 4 bytes: message_id

    Returns the payload as bytes.
    """
    recipient_id, message_type, content_size, message_content = data

    # ensure recipient_id is exactly 16 characters: truncate or pad with '\0'
    recipient_fixed = recipient_id[:16].ljust(16, '\0')
    recipient_bytes = recipient_fixed.encode('ascii', errors='ignore')

    message_id = generate_message_id()
    message_id_bytes = message_id.to_bytes(4, byteorder='big', signed=False)

    payload_bytes = recipient_bytes + message_id_bytes
    return payload_bytes
