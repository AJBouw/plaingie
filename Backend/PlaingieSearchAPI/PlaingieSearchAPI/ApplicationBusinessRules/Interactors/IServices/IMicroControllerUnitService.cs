using PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IServices;

public interface IMicroControllerUnitService
{
    Task<IEnumerable<MicroControllerUnit>> GetAllMicroControllerUnits();
}