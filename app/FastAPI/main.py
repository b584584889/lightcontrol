from typing import List

from fastapi import Depends, FastAPI, HTTPException
from sqlalchemy.orm import Session

from sql_app import crud, models, schemas
from sql_app.database import SessionLocal, engine

models.Base.metadata.create_all(bind=engine)

app = FastAPI()

# Dependency
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.get("/flyinfo/", response_model=List[schemas.FlyInfo], response_model_exclude_unset=True)
def read_flyinfos(skip: int = 0, limit: int = 100, db: Session = Depends(get_db)):
    items = crud.get_FlyInfo(db)
    return items
