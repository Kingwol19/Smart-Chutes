from pydantic import BaseModel

class SortingAudit(BaseModel):
    material: str
    purity_confirmed: bool
    risk_factor: str  # e.g., "Half-full liquid detected"
    revenue_impact: float