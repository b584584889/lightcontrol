from sqlalchemy import Boolean, Column, ForeignKey, Integer, String
from sqlalchemy.orm import relationship

from .database import Base
import json

class FlyInfo(Base):
    __tablename__ = "FlyInfo"

    FDATE = Column(String, primary_key=True, index=True)
    airLineNum = Column(String, primary_key=True, index=True)
    airLineCode = Column(String, primary_key=True, index=True)
    expectTime = Column(String)
    type = Column(String)
    jsonvalue = Column(String)
    triggerTime = Column(String)
    maingroupid = Column(Integer)
    datetime = Column(String)

    def to_dict(self):
        jsonObj = json.loads(self.jsonvalue)
        if self.triggerTime == None:
            jsonObj['triggerTime'] = ''
        else:
            jsonObj['triggerTime'] = self.triggerTime
        return jsonObj
