



def get_port(file_path):
    try:
        with open(file_path, 'r') as file:
            port = file.read().strip()
            port = int(port)

            return port
    except (FileNotFoundError, ValueError) as e:
        print(f"Error: {e}. Returning default port 1234.")
        return 1234


def main():
    port = get_port('myport.info')  
    print(f"Using port: {port}")

if __name__ == "__main__":
    main()