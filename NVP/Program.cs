using NVP.endPoints;
using NVP.Infrastructure;
using NVP.Services;
using Raven.Client.Documents;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
// Learn more about configuring OpenAPI at https://aka.ms/aspnet/openapi
builder.Services.AddOpenApi();
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

var app = builder.Build();

// Configure the HTTP request pipeline.

app.MapAuthEndpoints();
app.MapPackageEndpoints();

app.Run();
