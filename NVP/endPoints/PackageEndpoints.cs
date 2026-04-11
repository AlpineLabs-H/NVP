using System.Text.Json.Nodes;
using Microsoft.AspNetCore.Mvc;
using NVP.Models;
using NVP.Services;
using NVP.Infrastructure;
using Raven.Client.Documents;

namespace NVP.endPoints;

public static class PackageEndpoints
{
    private static ApiKeyDocument? ValidateApiKey(string? apiKeyHeader, IDocumentStore store)
    {
        if (string.IsNullOrEmpty(apiKeyHeader))
            return null;

        using var session = store.OpenSession();
        var doc = session.Query<ApiKeyDocument>()
            .Where(x => x.Key == apiKeyHeader)
            .FirstOrDefault();

        return doc;
    }

    public static void MapPackageEndpoints(this WebApplication app)
    {
        app.MapPost("/package/add", (
            [FromHeader(Name = "X-Api-Key")] string? apiKey,
            [FromBody] package package1,
            [FromServices] IDocumentStore store) =>
        {
            if (ValidateApiKey(apiKey, store) is null)
                return Results.Unauthorized();

            Tools.Save(package1);
            return Results.Ok("Package added successfully");
        });

        app.MapGet("/package/find/{name}/{OS}/{NVD}", (
            string name,
            string OS,
            string NVD,
            [FromHeader(Name = "X-Api-Key")] string? apiKey,
            [FromServices] IDocumentStore store,
            [FromServices] RavenDbService ravenDbService) =>
        {
            if (ValidateApiKey(apiKey, store) is null)
                return Results.Unauthorized();

            using var session = ravenDbService.OpenSession();
            var packages = session.Advanced
                .DocumentQuery<package>()
                .WhereEquals(x => x.name, name)
                .WhereEquals(x => x.OS, OS)
                .WhereEquals(x => x.NVD, NVD)
                .ToList();

            return Results.Ok(packages);
        });
    }
}