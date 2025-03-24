
import uuid

# in-memory storage for messages.
# keys are recipient IDs (ASCII strings) and values are lists of message records.
MESSAGE_STORAGE = {}


def generate_message_id():
    return int(uuid.uuid4().hex[:8], 16)


def decode_message_data(data):
    """
    decodes the raw message data.

    expected format of data:
      - 16 bytes: recipient_id (ASCII, padded or truncated)
      - 1 byte: message_type (e.g., 3 for text)
      - 4 bytes: content_size (big-endian integer)
      - n bytes: message_content (UTF-8 encoded)

    returns a dictionary with:
      - 'recipient_id'
      - 'message_type'
      - 'content_size'
      - 'message_content'

    raises a ValueError if the data is incomplete.
    """
    if len(data) < 21:
        raise ValueError("Data too short for a valid message payload.")

    # extract and decode the recipient ID.
    recipient_id_bytes = data[:16]
    recipient_id = recipient_id_bytes.decode('ascii', errors='ignore').rstrip('\0')

    message_type = data[16]

    # extract content size (4 bytes, big-endian, from index 17 to 21)
    content_size = int.from_bytes(data[17:21], byteorder='big')

    if len(data) < 21 + content_size:
        raise ValueError(f"Incomplete message content: expected {21 + content_size} bytes, got {len(data)}")

    # extract and decode the message content.
    message_content_bytes = data[21:21+content_size]
    message_content = message_content_bytes.decode('utf-8', errors='replace')

    return {
        'recipient_id': recipient_id,
        'message_type': message_type,
        'content_size': content_size,
        'message_content': message_content
    }


def save_to_message_storage(sender_id, data):
    """
    Saves a message into MESSAGE_STORAGE.

    parameters:
      - sender_id: The ID of the sender (string).
      - data: Raw bytes of the message payload (format defined in decode_message_data).

    function decodes the data, generates a new message ID, and creates a message record.
    record is then saved in MESSAGE_STORAGE under the recipient's ID.

    returns the message record (a dictionary).
    """
    try:
        # Decode the incoming message data.
        decoded = decode_message_data(data)

        # Generate a new message ID.
        message_id = generate_message_id()

        # Build a message record.
        message_record = {
            'sender_id': sender_id,
            'message_id': message_id,
            'recipient_id': decoded['recipient_id'],
            'message': decoded['message_content'],
            'message_type': decoded['message_type']
        }

        # Save the message record in storage.
        if decoded['recipient_id'] not in MESSAGE_STORAGE:
            MESSAGE_STORAGE[decoded['recipient_id']] = []
        MESSAGE_STORAGE[decoded['recipient_id']].append(message_record)

        return message_record
    except Exception as e:
        print("Error saving message to storage:", e)


# retrieve messages for a given recipient.
def get_messages_for_recipient(recipient_id):
    """
    Returns a list of message records for the given recipient ID.
    """
    return MESSAGE_STORAGE.get(recipient_id, [])


