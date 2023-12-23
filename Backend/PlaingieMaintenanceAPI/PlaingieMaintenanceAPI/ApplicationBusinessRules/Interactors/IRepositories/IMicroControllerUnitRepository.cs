using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IRepositories;

public interface IMicroControllerUnitRepository
{
    Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit);
    Task<MicroControllerUnit> GetMicroControllerUnitByMqttClientUId(string mqttClientUId);
}