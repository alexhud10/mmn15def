1. **config.h / config.cpp**  
   - Responsible for reading, storing, and managing application settings, such as the server IP address and port number.

2. **network.h / network.cpp**  
   - Handles all networking tasks, including establishing a connection to the server, sending/receiving data, and closing the connection.

3. **client.cpp**  
   - Contains the main function and orchestrates the overall client workflow, using functions from `config` and `network` to run the application.
