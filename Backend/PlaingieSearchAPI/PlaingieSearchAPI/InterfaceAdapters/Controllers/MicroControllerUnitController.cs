using Microsoft.AspNetCore.Mvc;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IServices;

namespace PlaingieSearchAPI.InterfaceAdapters.Controllers;

[ApiController]
[Route("plaingie/plaingie-search-api/[controller]")]
public class MicroControllerUnitController : ControllerBase
{
    private readonly IMicroControllerUnitService _microControllerUnitService;

    public MicroControllerUnitController(IMicroControllerUnitService microControllerUnitService)
    {
        _microControllerUnitService = microControllerUnitService;
    }

    [HttpGet]
    public async Task<IActionResult> GetMicroControllerUnits()
    {
        var result = await _microControllerUnitService.GetAllMicroControllerUnits();

        return Ok(result);
    }
}