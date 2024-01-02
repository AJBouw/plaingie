using Dapper;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IDataContext;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieMaintenanceAPI.EnterpriseBusinessRules.Entities;

namespace PlaingieMaintenanceAPI.InterfaceAdapters.Repositories;

public class MicroControllerUnitRepository : IMicroControllerUnitRepository
{
    private readonly IDatabaseContext _dbContext;

    public MicroControllerUnitRepository(IDatabaseContext dbContext)
    {
        _dbContext = dbContext;
    }

    public async Task CreateMicroControllerUnit(MicroControllerUnit microControllerUnit)
    {
        string sql = 
            "INSERT INTO " +
            "micro_controller_unit(mqtt_client_uid, mac_id, ip_id) " +
            "VALUES (@MqttClientUId, @MacId, @IpId)";
        
        using var db = _dbContext.GetConnection();
        var addedMicroControllerUnit = db.ExecuteAsync(sql, microControllerUnit);
        
        await addedMicroControllerUnit;
    }
    
    public async Task<MicroControllerUnit> GetMicroControllerUnitByMacId(string macId)
    {
        string sql = 
            "SELECT * " +
            "FROM micro_controller_unit " +
            "WHERE mac_id = @macId";
        
        using var db = _dbContext.GetConnection();
        var microControllerUnit = db.QuerySingleOrDefaultAsync<MicroControllerUnit>(sql, new { macId });

        return await microControllerUnit;
    }
    
    public async Task<MicroControllerUnit> GetMicroControllerUnitByMacIdMqttClientUId(string macId, string mqttClientUId)
    {
        string sql = 
            "SELECT * " +
            "FROM micro_controller_unit " +
            "WHERE mac_id = @macId AND " +
            "mqtt_client_uid = @mqttClientUId";
        
        using var db = _dbContext.GetConnection();
        var microControllerUnit = db.QuerySingleOrDefaultAsync<MicroControllerUnit>(sql, new { macId, mqttClientUId });

        return await microControllerUnit;
    }
    
    public async Task<MicroControllerUnit> GetMicroControllerUnitByMqttClientUId(string mqttClientUId)
    {
        string sql = 
            "SELECT * " +
            "FROM micro_controller_unit " +
            "WHERE mqtt_client_uid = @mqttClientUId";
        
        using var db = _dbContext.GetConnection();
        var microControllerUnit = db.QuerySingleOrDefaultAsync<MicroControllerUnit>(sql, new { mqttClientUId });

        return await microControllerUnit;
    }
}
