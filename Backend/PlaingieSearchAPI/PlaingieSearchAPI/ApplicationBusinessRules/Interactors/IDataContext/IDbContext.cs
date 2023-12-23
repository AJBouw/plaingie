using System.Data;

namespace PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IDataContext;

public interface IDbContext
{
    IDbConnection GetConnection();
}