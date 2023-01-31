
import json
from sqlalchemy.orm import Session

from . import models, schemas

def get_FlyInfo(db: Session):
    FlyInfos = db.query(models.FlyInfo).filter(models.FlyInfo.FDATE == '2021-01-18').order_by(models.FlyInfo.expectTime.asc()).all()
    return [ flyinfo.to_dict() for flyinfo in FlyInfos ]
