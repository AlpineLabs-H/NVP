using NVP.endPoints;
using NVP.Infrastructure;
using NVP.Services;
using Raven.Client.Documents;
using AspNet.Security.OAuth.GitHub;
using Microsoft.AspNetCore.Authentication.Cookies;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddSingleton<IDocumentStore>(sp =>
{
    var store = new DocumentStore
    {
        Urls = new[] { "http://192.168.1.20:8080" },
        Database = "NVP"
    };
    return store.Initialize();
});

builder.Services.AddScoped<RavenDbService>();
builder.Services.AddScoped<Tools>();

builder.Services.AddAuthentication(options =>
    {
        options.DefaultScheme = CookieAuthenticationDefaults.AuthenticationScheme;
    })
    .AddCookie()
    .AddGitHub(options =>
    {
        options.ClientId = builder.Configuration["GitHub:ClientId"]!;
        options.ClientSecret = builder.Configuration["GitHub:ClientSecret"]!;
        options.CallbackPath = "/auth/callback";
        options.Scope.Add("read:user");
    });

builder.Services.AddAuthorization();

var app = builder.Build();

app.UseAuthentication();
app.UseAuthorization();

app.MapAuthEndpoints();
app.MapPackageEndpoints();

app.Run();