using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IServices;

public interface IMicroControllerUnitService
{
    Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit);
}
