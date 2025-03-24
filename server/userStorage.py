# user storage will have the below func:
# store users info: id, user name and public key  in RAM memory
# user_data contains: user name, id and public key

class UserStorage:
    def __init__(self):
        self.users = []  # list to store user data as dictionaries

    def save_user_data(self, user_data):
        """store user data in-memory."""
        self.users.append(user_data)  # add user data to the in-memory list

    def load_user_data(self):
        """return all user data stored in memory."""
        return self.users

    def get_user_by_id(self, user_id):
        """retrieve user data by user_id."""
        for user in self.users:
            if user['user_id'] == user_id:
                return user
        return None  # return None if user is not found

    def username_exists(self, username):
        """check if the username already exists in storage."""
        for user in self.users:
            if user['username'] == username:
                return True
        return False  # return False if username is not found

    def remove_user(self, user_id):
        """remove a user from the in-memory list by user ID."""
        self.users = [user for user in self.users if user['user_id'] != user_id]

    def clear_all_users(self):
        """clear all user data from memory."""
        self.users.clear()



