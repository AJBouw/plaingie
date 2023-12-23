using Dapper;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IDataContext;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieSearchAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieSearchAPI.InterfaceAdapters.Repositories;

public class MicroControllerUnitRepository : IMicroControllerUnitRepository
{
    private readonly IDbContext _dbContext;

    public MicroControllerUnitRepository(IDbContext dbContext)
    {
        _dbContext = dbContext;
    }
    
    public async Task<IEnumerable<MicroControllerUnit>> GetAllMicroControllerUnits()
    {
        string sql = "SELECT " +
                        "uid as UId," +
                        "created_at as CreatedAt," +
                        "updated_at as UpdatedAt," +
                        "ip_id as IpId," +
                        "mac_id as MacId," +
                        "mqtt_client_uid as MqttClientUId " +
                     "FROM micro_controller_unit " +
                     "ORDER BY uid";
        
        using var db = _dbContext.GetConnection();
        var microControllerUnits = await db.QueryAsync<MicroControllerUnit>(sql);
        
        return microControllerUnits;
    }
}