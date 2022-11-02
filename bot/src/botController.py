from models.chrome_browser import *
from models.firefox_browser import *
from models.tor_browser import *
from models.Bot import Bot
from models.myEnum import BOT_TYPE
import sys

class BotController:
    
    bots = list();
    def __init__(self):
        self.createBot(BOT_TYPE.FIREFOX_BOT, sys.argv[1])
        # self.createBot(BOT_TYPE.FIREFOX_BOT, sys.argv[1])
    
    def set_botType(self, type):
        self._botType = type;
    def get_botType(self):
        return self._botType;
       
    def set_driver(self, driver):
        self._driver = driver
    def get_driver(self):
        return self._driver
    
    def createBot(self, bot_type: BOT_TYPE, siteUrl):
        self.bots.append(Bot(bot_type,rootUrl=siteUrl, awake=True))
        
    def run(self):
        if(len(self.bots)==0):
            print("No bot exist")
            return
        
        for bot in self.bots:
            bot.browse()
        
        
        
        
    

controller = BotController()
controller.run();

