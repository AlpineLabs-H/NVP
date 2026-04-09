namespace NVP.endPoints;

public static class AuthEndpoints
{
    public static void MapAuthEndpoints(this WebApplication app)
    {
        app.MapGet("/auth/test", () => "hello");
        app.MapPost("/auth/init", () => { /* ... */ });
    }
    
}