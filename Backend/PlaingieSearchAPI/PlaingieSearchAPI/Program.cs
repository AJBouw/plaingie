using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.Identity.Web;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IDataContext;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IRepositories;
using PlaingieSearchAPI.ApplicationBusinessRules.Interactors.IServices;
using PlaingieSearchAPI.ApplicationBusinessRules.UseCases;
using PlaingieSearchAPI.InterfaceAdapters.DataContext;
using PlaingieSearchAPI.InterfaceAdapters.Repositories;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
    .AddMicrosoftIdentityWebApi(builder.Configuration.GetSection("AzureAd"));

builder.Services.AddControllers();
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddSingleton<IDbContext, DbContext>();
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