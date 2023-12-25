using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IServices;
using PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieSearchAPI.ApplicationBusinessRules.UseCases;

public class MicroControllerUnitService : IMicroControllerUnitService
{
    private readonly IMicroControllerUnitRepository _microControllerUnitRepository;

    public MicroControllerUnitService(IMicroControllerUnitRepository microControllerUnitRepository)
    {
        _microControllerUnitRepository = microControllerUnitRepository;
    }

    public async Task<IEnumerable<MicroControllerUnit>> GetAllMicroControllerUnits()
    {
        return await _microControllerUnitRepository.GetAllMicroControllerUnits();
    }
}