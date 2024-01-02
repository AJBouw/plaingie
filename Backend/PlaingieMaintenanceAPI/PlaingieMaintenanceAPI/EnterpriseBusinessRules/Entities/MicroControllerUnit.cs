using System.ComponentModel.DataAnnotations;
using System.Diagnostics.CodeAnalysis;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Common;

namespace PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

public class MicroControllerUnit : BaseEntity
{
    [MaxLength(46)]
    [MinLength(12)]
    public string IpId { get; set; }
    
    
    [MaxLength(17)]
    [MinLength(12)]
    [Required]
    public string MacId { get; set; }
    
    [MaxLength(50)]
    [MinLength(8)]
    [Required]
    public string MqttClientUId { get; set; }
}
