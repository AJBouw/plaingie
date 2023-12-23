using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IService;

public interface IMicroControllerUnitService
{
    Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit);
}