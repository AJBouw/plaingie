using System.Data;

namespace PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IDataContext;

public interface IDatabaseContext
{
    IDbConnection GetConnection();
}
