namespace NVP.Models;

public class ApiKeyDocument
{
    public string Id { get; set; }
    public string Key { get; set; }
    public string GithubId { get; set; }
    public string GithubUsername { get; set; }
    public DateTime CreatedAt { get; set; }
}