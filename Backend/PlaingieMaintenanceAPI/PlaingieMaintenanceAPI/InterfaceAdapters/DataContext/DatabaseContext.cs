using System.Data;
using Npgsql;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IDataContext;

namespace PlaingieMaintenanceAPI.InterfaceAdapters.DataContext;

public class DatabaseContext : IDatabaseContext
{
    private readonly IConfiguration _configuration;
    
    public DatabaseContext(IConfiguration configuration)
    {
        _configuration = configuration;
    }

    public IDbConnection GetConnection()
    {
        return new NpgsqlConnection(_configuration.GetConnectionString("dbDEV"));
    }
}
