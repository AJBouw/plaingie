using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class ServoData : BaseEntity
{
    public float CurrentPosition { get; set; }
    public float StartPosition { get; set; }
    
    public IoTDevice IoTDevice { get; set; }
}