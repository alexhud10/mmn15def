1. **server.py**
Responsible for: Starting the server, accepting client connections, and managing the overall server workflow.
Functionality:
Listens on a specific IP address and port for incoming client connections.
Routes messages to appropriate handlers from message_handler.py.
Sends responses back to the client.
2. **config.py**
Responsible for: Reading, storing, and managing application settings, such as the server IP address and port number.
Functionality:
Loads server IP and port from the server.info configuration file.
Provides methods to retrieve the IP and port settings (getIP() and getPort()).
If the configuration file is missing or incorrectly formatted, defaults are used.
3. **message_handler.py**
Responsible for: Processing and routing messages received from clients, handling different types of messages, and sending appropriate responses.
Functionality:
Parses incoming messages from clients.
Handles requests and determines the appropriate action (such as sending back a response).
Handles different types of messages (requests, responses, etc.) based on predefined rules.
Logs and manages errors related to message processing.