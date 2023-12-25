using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class LightSensorData : BaseEntity
{
    public int LightIntensity { get; set; }
    
    public IoTDevice IoTDevice { get; set; }
}