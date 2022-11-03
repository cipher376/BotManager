from logging import root
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium import webdriver
from models.myEnum import BOT_TYPE
from models.credentials import Credentials
from models.firefox_browser import FirefoxBrowser
from models.tor_browser import TorBrowser

from models.config import *
import random
from models.page import Page
from itertools import chain


class Bot:

    siteCredentials: Credentials = None
    driver = None
    rootUrl: str
    pages = []
    currentLevel: int = 0
    currentPage: Page;
    
    awake: bool;
    
    tabHandles = []
    
    def __init__(self, bot_type: BOT_TYPE, credentials: Credentials = None, 
                 rootUrl = None, awake=False):
        
        self.siteCredentials = credentials
        self.rootUrl = rootUrl
        self.awake = awake; # New bot sleeps
        self.currentPage =  None
        
        if(bot_type == BOT_TYPE.FIREFOX_BOT):
            browser = FirefoxBrowser(MOZILLA_BINARY_PATH)
            self.driver = webdriver.Firefox(options=browser.options)

        elif (bot_type == BOT_TYPE.CHROME_BOT):
            self.driver = webdriver.Chrome()

        elif (bot_type == BOT_TYPE.OPERA_BOT):
            self.driver = webdriver.WebKitGTK

        elif (bot_type == BOT_TYPE.IE_BOT):
            self.driver = webdriver.Ie

        elif(bot_type == BOT_TYPE.EDGE_BOT):
            self.driver = webdriver.ChromiumEdge

        elif(bot_type == BOT_TYPE.TOR_BOT):
            self.driver = TorBrowser().getDriver();

    def browse(self):

        # Initial home page setup
        self.currentLevel  = 0;

        # pageLoaded = False
        
        # while(not pageLoaded):

        #visit the page
        while True:
            try:
                self.driver.implicitly_wait(random.randrange(10, 30))
                self.driver.get(self.rootUrl)
                self.driver.implicitly_wait(random.randrange(20, 30))

                # pageLoaded = True;
                break
            except:
                print("Unable to load url: ", self.rootUrl);
                self.driver.implicitly_wait(random.randrange(1, 20))

                # pageLoaded = False;
            
        
    
        # Browse internal links or ads
        while self.awake:
            try:
                # Randomize scrolling behaviours
                windowHeight = self.driver.execute_script("return document.body.scrollHeight")
                print("Window inner hight: ", windowHeight);
                if(windowHeight <=0):
                    windowHeight = 1;

                scrollTo = int(windowHeight/random.randrange(1, windowHeight))
                # self.driver.execute_script("window.scroll(0,"+str(scrollTo)+")")
                self.driver.implicitly_wait(random.randrange(0, 10))
            except Exception:
                pass
            
            self.buildPage(); 
            print("Links")
            if(self.currentPage):
                print( self.currentPage.links)
                print()
                print( self.currentPage.ads_frames)
                print()

            


            links = [] 
            ads_len = 0;
            try:
                
                # links = self.currentPage.ads_frames;
                print("--------------------------------------")
                print(self.currentPage.ads_frames)
                print("--------------------------------------")

                if(len(self.currentPage.ads_frames)>0 and len(self.currentPage.links)>0):
                    ads_len = int(len(self.currentPage.links)/len(self.currentPage.ads_frames))
                
                print(ads_len) 

                self.currentPage.ads_frames = [self.currentPage.ads_frames]*(ads_len+1)
                links =  self.currentPage.ads_frames+ self.currentPage.links;
                print("Total ads links: ", ads_len)

                if(ads_len == 0):
                    break
            except :
                print("Invalid links")
                break
                
            
            # Randomly select links or ads to visit
            length = len(links)
            if(length == 0):
                continue;
            pos = random.randrange(0, length);
            link = links[pos];
            
           
            
            if(pos < len(self.currentPage.ads_frames)):
                # browse as an ad
                # click and open new tab
                try:
                    # link.click();
                    handle = self.openTab(link);
                    # switch to tab
                    if(handle):
                        self.driver.implicitly_wait(random.randrange(0, 10))
                    
                        self.driver.switch_to.window(handle)
                    #     # wait
                        ad_links = self.findInternalLinks();
                        # ad_links = self.findInternalLinks();
                        if(len(ad_links)>0):
                            # actions = ActionChains(self.driver)
                            # actions.scroll_to_element(link).move_to_element(link)
                            # actions.perform()
                            self.driver.execute_script("window.scroll("+str(link.location['x'])+","+str(link.location['y'])+")")
                            
                            self.driver.implicitly_wait(random.randrange(0, 20))
                            self.driver.close() #close current window
                    
                    print("Ads clicked")
                
                except Exception as msg:
                    print("Ads click failed")
                    print(msg)
                    print(link)
                    
                # wait for few minutes
                # scroll up and down
                # randomly click a link or stop
                
            elif(pos > len(self.currentPage.ads_frames)):
                try:
                    actions = ActionChains(self.driver)
                    # actions.scroll_to_element(link).move_to_element(link)
                    # actions.perform();
                    
                    print("Link location: ", link.location['y']);
                    self.driver.execute_script("window.scroll("+str(link.location['x'])+","+str(link.location['y'])+")")
                    self.driver.implicitly_wait(random.randrange(0, 5))

                    actions.click();
                    actions.perform();
                    actions.reset_actions();
                    
                    
                    # link.click()
                    
                    print("inner link clicked")
                
                except Exception as msg:
                    print("inner link click failed")
                    print()
                    print(msg)

                # get the current url 
                # url = self.driver.
                
            # Find the next page to navigate to
            # print(links);
            # break
            
        

        # assert "ben" in self.driver.title
        # elem = self.driver.find_element(By.NAME, "q")
        # elem.clear()
        # elem.send_keys("pycon")
        # elem.send_keys(Keys.RETURN)
        # assert "No results found." not in self.driver.page_source

        # print(ad_frames)
        # if(len(ad_frames) > 0):
        #     ad_pos = random.randrange(0, len(ad_frames))
        #     ad_frames[ad_pos].click()
        #     print("Add clicked")
        #     print(ad_pos)
        # self.driver.close()


    def sleep(self):
        pass

    def kill(self):
        self.driver.quit()

    def clickConsent(self):
        """
        looks for consent button
        """
        # self.driver.implicitly_wait(random.randrange(30, 60))
        try:
            consent_btn = self.driver.find_element(By.CLASS_NAME, value="fc-cta-consent")
            print(consent_btn)
            if(consent_btn):
                consent_btn.click()
                # self.driver.implicitly_wait(random.randrange(15, 30))
        except:
            print("No consent")
        
        

    def findAds(self):
           ads = self.driver.find_elements(By.TAG_NAME, value="iframe")
           if(type(ads) == list):
               return ads
           else: return [ads]

    
        # print()
        # print()
        # # print(links)

    def findInternalLinks(self):
        link = self.driver.find_elements(By.TAG_NAME, value="a")
        if(type(link) == list):
               return link
        else: return [link]               

    def forward(self):
        # check if it has already visited in links
        if(self.currentLevel < len(self.pages)):
            self.driver.forward()
            # self.driver.implicitly_wait(random.randrange(5, 30))
            self.currentLevel = self.currentLevel+1
            
    
    
    def buildPage(self):
        '''
        Construct new page object if doesn't exit and add to the browsing history
        '''
        
        try:
            url = self.driver.current_url; 


            # print(url)
            # print("_________________________________________")

            # Look for page in history
            levelIndex = 0;
            groupIndex = 0;

            page = Page(url, level= self.currentLevel)

            for item in self.pages:
                levelIndex = levelIndex+1;
                for p in item:
                    groupIndex = groupIndex+1;
                    p = Page(p);
                    if(p.url == url):
                        self.currentPage = p;



            # self.driver.implicitly_wait(random.randrange(15, 30))

            #check for cookie or session consent
            self.clickConsent()

            #get internal links
            page.links = self.findInternalLinks();

            #get ads
            page.ads_frames = self.findAds()

            # Find page level and save/update page
            if(len(self.pages) > self.currentLevel):
                group = self.pages[self.currentLevel];

                if( len(group) >0):
                    self.pages[self.currentLevel].append(page)
                else:
                    self.pages[self.currentLevel] = [page]
            else: 
                self.pages.append([page])       

            self.currentPage = page;

            print(page)
        
        except Exception as msg:
            print("Context changed, url error")
            print(msg)
    
    def switchToTab(self, handle):
        self.driver.switch_to.window(handle)
        
    def openTab(self, element):
        actions = ActionChains(self.driver)

        # actions.scroll_to_element(element).move_to_element(element)
        # actions.perform()

        element.location
        
        self.driver.implicitly_wait(random.randrange(0, 20))
                    
        actions.key_down(Keys.CONTROL).click(element).key_up(Keys.CONTROL);
        actions.perform()
        actions.reset_actions()
        
        for handle in self.driver.window_handles:
            if(not(handle in self.tabHandles)):
                self.tabHandles.append(handle);
                return handle
    
    def getTab(self, id: int):
        '''
        return handle with id
        1 means first tab
        '''
        if(id > 0):
            return (self.tabHandles[id-1])
    