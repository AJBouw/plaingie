using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IService;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.UseCases;

public class MicroControllerUnitService : IMicroControllerUnitService
{
    private readonly IMicroControllerUnitRepository _microControllerUnitRepository;

    public MicroControllerUnitService(IMicroControllerUnitRepository microControllerUnitRepository)
    {
        _microControllerUnitRepository = microControllerUnitRepository;
    }

    public async Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit)
    {
        if (await _microControllerUnitRepository.GetMicroControllerUnitByMqttClientUId(microControllerUnit.MqttClientUId) != null)
        {
            throw new ApplicationException("409 | Conflict | Micro Controller Unit already exists");
        }

        await _microControllerUnitRepository.CreateMicroControllerUnit(microControllerUnit);
    }
}