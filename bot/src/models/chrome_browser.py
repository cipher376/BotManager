
from selenium.webdriver.firefox.options import Options

class ChromeBrowser:
    options = Options;
    def __init__(self, binary_location):
        self.options.binary_location = binary_location;