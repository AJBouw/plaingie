using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IRepositories;

public interface IMicroControllerUnitRepository
{
    Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit);
    Task<MicroControllerUnit> GetMicroControllerUnitByMacId(string macId);
    Task<MicroControllerUnit> GetMicroControllerUnitByMacIdMqttClientUId(string macId, string mqttClientUId);
    Task<MicroControllerUnit> GetMicroControllerUnitByMqttClientUId(string mqttClientUId);
    
}
