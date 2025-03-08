# binary protocol implementation

import struct


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
