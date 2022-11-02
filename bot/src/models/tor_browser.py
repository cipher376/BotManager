from selenium.webdriver.firefox.options import Options
from tbselenium.tbdriver import TorBrowserDriver
from models.config import TOR_BINARY_PATH, TOR_BUNDLE_PATH

class TorBrowser:
    options = Options()
    binary_location = ""
    def __init__(self, binary_location=TOR_BINARY_PATH):
        self.options.binary_location = binary_location;
        self.binary_location = binary_location;
        
        # self.options.set_preference("profile", "TorBrowser/Data/Browser/profile.default");
        # self.options.set_preference("name", "Tor Browser");
        # self.options.set_preference("class", "Tor Browser");
        # self.options.set_preference("allow-remote", "")
        print(binary_location);

    def getDriver(self):
        with TorBrowserDriver(TOR_BUNDLE_PATH) as driver: return driver