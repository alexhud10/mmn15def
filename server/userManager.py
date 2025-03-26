
# registration of users, authentication, generate user id


import uuid
from userStorage import *
class UserManager:
    def __init__(self, user_storage):
        # Takes an instance of UserStorage
        self.user_storage = user_storage  # This will delegate to UserStorage for storing user data

    def register_user(self, username, public_key):
        """registers a new user by generating a user_id and storing in UserStorage."""
        user_id = uuid.uuid4().hex[:16]
        user_data = {
            'user_id': user_id,
            'username': username,
            'public_key': public_key if public_key else None,
        }
        self.user_storage.save_user_data(user_data)
        print(f"User '{username}' registered with ID {user_id}.")
        return True, user_id
'''
    def authenticate_user(self, user_id):
        """authenticates a user by checking if their user_id exists in UserStorage."""
        user = self.user_storage.get_user_by_id(user_id)
        if user:
            print(f"User {user_id} authenticated.")
            return True
        else:
            print(f"User {user_id} not found.")
            return False
'''
