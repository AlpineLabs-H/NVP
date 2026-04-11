using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Authentication.Cookies;
using NVP.Infrastructure;
using NVP.Models;
using NVP.Services;
using Raven.Client.Documents;
using System.Security.Claims;
using System.Security.Cryptography;
using Microsoft.AspNetCore.Mvc;

namespace NVP.endPoints;

public static class AuthEndpoints
{
    public static void MapAuthEndpoints(this WebApplication app)
    {
        // Step 1 — CLI hits this, browser opens, GitHub login happens
        app.MapGet("/auth/login", () =>
        {
            return Results.Challenge(
                new AuthenticationProperties { RedirectUri = "/auth/register" },
                new[] { "GitHub" }
            );
        });

        // Step 2 — GitHub redirects here automatically (don't call this yourself)
        // AspNet.Security.OAuth.GitHub handles this internally via CallbackPath

        // Step 3 — After callback, generate and return the API key
        app.MapGet("/auth/register", async (
            HttpContext context,
            [FromServices] IDocumentStore store) =>
        {
            // Check they actually logged in
            var result = await context.AuthenticateAsync(CookieAuthenticationDefaults.AuthenticationScheme);
            if (!result.Succeeded)
                return Results.Unauthorized();

            // Get their GitHub info from the claims
            var githubId = result.Principal?.FindFirstValue(ClaimTypes.NameIdentifier);
            var githubUsername = result.Principal?.FindFirstValue(ClaimTypes.Name);

            if (githubId == null)
                return Results.Unauthorized();

            using var session = store.OpenSession();

            // Check if they already have a key
            var existing = session.Query<ApiKeyDocument>()
                .Where(x => x.GithubId == githubId)
                .FirstOrDefault();

            if (existing != null)
                return Results.Ok(new { apiKey = existing.Key, username = githubUsername });

            // Generate a new API key
            string apiKey = Convert.ToHexString(RandomNumberGenerator.GetBytes(32)).ToLowerInvariant();

            var doc = new ApiKeyDocument
            {
                Id = $"ApiKeys/{githubId}",
                Key = apiKey,
                GithubId = githubId,
                GithubUsername = githubUsername,
                CreatedAt = DateTime.UtcNow
            };

            session.Store(doc);
            session.SaveChanges();

            return Results.Ok(new { apiKey, username = githubUsername });
        });
    }
}