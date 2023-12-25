using System.ComponentModel.DataAnnotations.Schema;
using PlaingieSearchAPI.EnterpriseBusinessRules.Common;

namespace PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

public class MicroControllerUnit : BaseEntity
{
    public string IpId { get; set; }
    public string MacId { get; set; }
    public string MqttClientUId { get; set; }
    
    public List<IoTDevice> IoTDevices { get; set; }
}