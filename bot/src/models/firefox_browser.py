from selenium.webdriver.firefox.options import Options


class FirefoxBrowser:
    options = Options()
    def __init__(self, binary_location):
        self.options.binary_location = binary_location;
        print(binary_location);