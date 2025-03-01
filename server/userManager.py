# registration of users, authentication, generate user id


import uuid
from userStorage import *
class UserManager:
    def __init__(self, user_storage):
        # Takes an instance of UserStorage
        self.user_storage = user_storage  # This will delegate to UserStorage for storing user data

    def register_user(self, username):
        """Registers a new user by generating a user_id and storing in UserStorage."""
        # Check if the username already exists in storage
        if self.user_storage.username_exists(username):
            print(f"Error: Username '{username}' is already taken.")
            return  # Return early if the username exists

        # If the username does not exist, proceed with registration
        user_id = str(uuid.uuid4())  # Generate a unique user ID
        user_data = {
            'user_id': user_id,
            'username': username,
            'public_key': None  # We'll add public key in future
        }
        # Delegate storing user data to UserStorage
        self.user_storage.save_user_data(user_data)
        print(f"User {username} with ID {user_id} registered.")

    def authenticate_user(self, user_id):
        """Authenticates a user by checking if their user_id exists in UserStorage."""
        user = self.user_storage.get_user_by_id(user_id)
        if user:
            print(f"User {user_id} authenticated.")
            return True
        else:
            print(f"User {user_id} not found.")
            return False

