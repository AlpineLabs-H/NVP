using NVP.Models;
using NVP.Services;
using NVP.Infrastructure;

namespace NVP.endPoints;

public static class PackageEndpoints
{
    
    public static void MapPackageEndpoints(this WebApplication app)
    {
        app.MapGet("/package/test", () =>
        {
            return new test { id = "test", RN = 123 };
        });

        app.MapPost("/package/add", (package package1) =>
        {
            Tools.Save(package1);
            return package1;
        });
        app.MapGet("/package/find/{name}/{OS}/{NVD}", (string name, string OS, string NVD, RavenDbService ravenDbService) =>
        {
            using (var session = ravenDbService.OpenSession())
            {
                List<package> packages = session.Advanced
                    .DocumentQuery<package>()
                    .WhereEquals(x => x.name, name)
                    .WhereEquals(x => x.OS, OS)
                    .WhereEquals(x => x.NVD, NVD)
                    .ToList();
                return packages;
            }
        });
    }
    
}