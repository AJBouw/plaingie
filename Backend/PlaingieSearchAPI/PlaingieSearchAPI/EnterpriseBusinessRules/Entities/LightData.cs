using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class LightData : BaseEntity
{
    public bool LightStatus { get; set; }
    public string OperatingMode { get; set; }
    public int VisibilityLevelBright { get; set; }
    public int VisibilityLevelDark { get; set; }
    public int VisibilityLevelDim { get; set; }
    
    public IoTDevice IoTDevice { get; set; }
}