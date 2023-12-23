namespace PlaingieSearchAPI.EnterpriseBusinessRules.Common;

public class BaseEntity
{
    public int UId { get; set; }
    public DateTimeOffset CreatedAt { get; set; }
    public DateTimeOffset? UpdatedAt { get; set; }
}