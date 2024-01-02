using Microsoft.AspNetCore.Mvc;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IServices;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Common;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.InterfaceAdapters.Controllers;

[ApiController]
[Route("plaingie/plaingie-maintenance-api/[controller]")]
public class MicroControllerUnitController : ControllerBase
{
    private readonly ILogger<MicroControllerUnitController> _logger;
    private readonly IMicroControllerUnitService _microControllerUnitService;

    public MicroControllerUnitController(ILogger<MicroControllerUnitController> logger, IMicroControllerUnitService microControllerUnitService)
    {
        _logger = logger;
        _microControllerUnitService = microControllerUnitService;
    }

    [HttpPost]
    public async Task<IActionResult> CreateMicroControllerUnit([FromBody] MicroControllerUnit microControllerUnit)
    {
        if (!ModelState.IsValid)
        {
            return StatusCode(StatusCodes.Status400BadRequest);
        }

        try
        {
            await _microControllerUnitService.CreateMicroControllerUnit(microControllerUnit);
            return Ok();
        }
        catch (InvalidOperationException ex)
        {
            return StatusCode(StatusCodes.Status409Conflict, ex.Message);
        }
        catch (Exception ex)
        {
            return StatusCode(StatusCodes.Status500InternalServerError, ex.Message);
        }
    }
}
