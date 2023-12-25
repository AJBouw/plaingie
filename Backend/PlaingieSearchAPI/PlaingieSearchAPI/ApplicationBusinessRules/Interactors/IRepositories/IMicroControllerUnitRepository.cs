using PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IRepositories;

public interface IMicroControllerUnitRepository
{
    Task<IEnumerable<MicroControllerUnit>> GetAllMicroControllerUnits();
}