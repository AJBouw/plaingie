FROM mcr.microsoft.com/dotnet/aspnet:7.0 AS base
WORKDIR /app
EXPOSE 80
EXPOSE 443

FROM mcr.microsoft.com/dotnet/sdk:7.0 AS build
WORKDIR /src
COPY ["PlaingieMaintenanceAPI/PlaingieMaintenanceAPI.csproj", "PlaingieMaintenanceAPI/"]
RUN dotnet restore "PlaingieMaintenanceAPI/PlaingieMaintenanceAPI.csproj"
COPY . .
WORKDIR "/src/PlaingieMaintenanceAPI"
RUN dotnet build "PlaingieMaintenanceAPI.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "PlaingieMaintenanceAPI.csproj" -c Release -o /app/publish /p:UseAppHost=false

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "PlaingieMaintenanceAPI.dll"]
