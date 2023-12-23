using Microsoft.AspNetCore.Mvc;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IService;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.InterfaceAdapters.Controllers;

[ApiController]
[Route("plaingie/plaingie-maintenance-api/[controller]")]
public class MicroControllerUnitController : ControllerBase
{
    private readonly IMicroControllerUnitService _microControllerUnitService;

    public MicroControllerUnitController(IMicroControllerUnitService microControllerUnitService)
    {
        _microControllerUnitService = microControllerUnitService;
    }

    [HttpPost]
    public async Task<IActionResult> CreateMicroControllerUnit(MicroControllerUnit microControllerUnit)
    {
        await _microControllerUnitService.CreateMicroControllerUnit(microControllerUnit);
        return Ok(new { message = "Micro Controller Unit created" });
    }
}