using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.Identity.Web;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IDataContext;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.Interactors.IServices;
using PlaingieMaintenanceAPI.ApplicationBusinessRules.UseCases;
using PlaingieMaintenanceAPI.InterfaceAdapters.DataContext;
using PlaingieMaintenanceAPI.InterfaceAdapters.Repositories;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
    .AddMicrosoftIdentityWebApi(builder.Configuration.GetSection("AzureAd"));

builder.Services.AddControllers();
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddSingleton<IDatabaseContext, DatabaseContext>();
builder.Services.AddScoped<IMicroControllerUnitService, MicroControllerUnitService>();
builder.Services.AddScoped<IMicroControllerUnitRepository, MicroControllerUnitRepository>();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseHttpsRedirection();

app.UseAuthorization();

app.MapControllers();

app.Run();