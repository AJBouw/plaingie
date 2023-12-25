using System.Data;
using Npgsql;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IDataContext;

namespace PlaingieSearchAPI.InterfaceAdapters.DataContext;

public class DbContext : IDbContext
{
    private readonly IConfiguration _configuration;

    public DbContext(IConfiguration configuration)
    {
        _configuration = configuration;
    }

    public IDbConnection GetConnection()
    {
        return new NpgsqlConnection(_configuration.GetConnectionString("dbDEV"));
    }
}