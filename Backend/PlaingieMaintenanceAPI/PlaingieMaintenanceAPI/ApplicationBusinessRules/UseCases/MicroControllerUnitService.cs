using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IServices;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.UseCases;

public class MicroControllerUnitService : IMicroControllerUnitService
{
    private readonly ILogger<MicroControllerUnitService> _logger;
    private readonly IMicroControllerUnitRepository _microControllerUnitRepository;

    public MicroControllerUnitService(ILogger<MicroControllerUnitService> logger, IMicroControllerUnitRepository microControllerUnitRepository)
    {
        _logger = logger;
        _microControllerUnitRepository = microControllerUnitRepository;
    }

    public async Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit)
    {
        var existingMacId = await _microControllerUnitRepository.GetMicroControllerUnitByMacId(microControllerUnit.MacId);
        var existingMicroControllerUnit = await _microControllerUnitRepository.GetMicroControllerUnitByMacIdMqttClientUId(microControllerUnit.MacId, microControllerUnit.MqttClientUId);
        var existingMqttClientUId = await _microControllerUnitRepository.GetMicroControllerUnitByMqttClientUId(microControllerUnit.MqttClientUId);

        if ((existingMicroControllerUnit != null) || (existingMacId != null) || existingMqttClientUId != null)
        {
            throw new InvalidOperationException("Conflict");
        }
        
        await _microControllerUnitRepository.CreateMicroControllerUnit(microControllerUnit);
    }
}
