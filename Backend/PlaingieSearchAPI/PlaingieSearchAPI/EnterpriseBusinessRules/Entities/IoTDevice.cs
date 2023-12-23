using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class IoTDevice : BaseEntity
{
    public string Category { get; set; }
    public string Gpio { get; set; }
    public string Identifier { get; set; }
    public bool IsActive { get; set; }
    public string LocationLabel { get; set; }
    public string? LocationDescription { get; set; }
    
    public MicroControllerUnit MicroControllerUnit { get; set; }
    
    public IEnumerable<LightData>? LightData { get; set; }
    public IEnumerable<LightSensorData>? LightSensorData { get; set; }
    public IEnumerable<MotionSensorData>? MotionSensorData { get; set; }
    public IEnumerable<ServoData>? ServoData { get; set; }
    public IEnumerable<WebcamData>? WebcamData { get; set; }
}