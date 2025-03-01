# user storage will have the below func:
# store users info: id, user name and public key  in RAM memory
# user_data countans: user name, id and public key

class UserStorage:
    def __init__(self):
        self.users = []  # List to store user data as dictionaries

    def save_user_data(self, user_data):
        """Store user data in-memory."""
        self.users.append(user_data)  # Add user data to the in-memory list

    def load_user_data(self):
        """Return all user data stored in memory."""
        return self.users

    def get_user_by_id(self, user_id):
        """Retrieve user data by user_id."""
        for user in self.users:
            if user['user_id'] == user_id:
                return user
        return None  # Return None if user is not found

    def username_exists(self, username):
        """Check if the username already exists in storage."""
        for user in self.users:
            if user['username'] == username:
                return True
        return False  # Return False if username is not found

    def remove_user(self, user_id):
        """Remove a user from the in-memory list by user ID."""
        self.users = [user for user in self.users if user['user_id'] != user_id]

    def clear_all_users(self):
        """Clear all user data from memory."""
        self.users.clear()



