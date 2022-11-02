class Credentials:
    username = ''
    password = ''
    persistence = False
    
    def __init__(self): 
        pass
    
    def set_username(self, username):
        self.username = username;
        
    def get_username(self):
        return self.username