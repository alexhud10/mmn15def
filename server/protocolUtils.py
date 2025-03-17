# binary protocol implementation

import struct

# decode packet arrived from client
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


'''sending packets to client'''
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

