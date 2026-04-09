using Raven.Client.Documents.Session;
using Raven.Client.Documents;
using NVP;

namespace NVP.Services;

public class Tools
{
    private static readonly IDocumentStore store = new DocumentStore
    {
        Urls = new[] { "http://192.168.1.20:8080" },
        Database = "NVP"
    }.Initialize();

    public static void Save(object model)
    {
        using (IDocumentSession session = store.OpenSession())
        {
            session.Store(model);
            session.SaveChanges();
        }
    }
}