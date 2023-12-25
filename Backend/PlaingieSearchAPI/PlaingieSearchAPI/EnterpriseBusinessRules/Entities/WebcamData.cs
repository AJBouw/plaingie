using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class WebcamData : BaseEntity
{
    public string? State { get; set; }
    public bool Status { get; set; }
    
    public IoTDevice IoTDevice { get; set; }
}