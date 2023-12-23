using System.Data;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IDataContext;

public interface IDbContext
{
    IDbConnection GetConnection();
}