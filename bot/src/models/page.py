import uuid;


class Page:
    pageId: int
    url: str;
    ads_frames =list[str];
    level:int;
    links = list[str]
    
    def __init__(self,url, level=0, ads_frames=[]):
        self.url = url
        self.ads_frames = ads_frames
        self.level = level
        self.links = []
        self.uid = uuid.uuid4()