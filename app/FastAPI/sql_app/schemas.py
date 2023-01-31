
from typing import List, Optional

from pydantic import BaseModel

class FlyInfoBase(BaseModel):
    type: str
    FDATE: str

class FlyInfoCreate(FlyInfoBase):
    pass

class FlyInfo(FlyInfoBase):
    type: str
    FDATE: str
    airLineCode: str
    airLineNum: str
    airPlaneType: str
    STD: Optional[str] = None
    amhsETD: Optional[str] = None
    ritETD: Optional[str] = None
    ATD: Optional[str] = None
    ArrivalAirport: Optional[str] = None
    STA: Optional[str] = None
    amhsETA: Optional[str] = None
    ritETA: Optional[str] = None
    ATA: Optional[str] = None
    DepartureAirport: Optional[str] = None
    triggerTime: Optional[str] = None

    class Config:
        orm_mode = True
