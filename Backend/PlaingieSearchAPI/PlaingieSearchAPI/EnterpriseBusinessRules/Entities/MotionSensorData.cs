using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class MotionSensorData : BaseEntity
{
    public bool MotionIsDetected { get; set; }
    
    public IoTDevice IoTDevice { get; set; }
}