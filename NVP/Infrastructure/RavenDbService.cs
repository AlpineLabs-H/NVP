using Raven.Client.Documents;
using Raven.Client.Documents.Session;

namespace NVP.Infrastructure;

public class RavenDbService
{
    private readonly IDocumentStore _store;

    public RavenDbService(IDocumentStore store)
    {
        _store = store;
    }

    public IDocumentSession OpenSession()
    {
        return _store.OpenSession();
    }

    public IAsyncDocumentSession OpenAsyncSession()
    {
        return _store.OpenAsyncSession();
    }
}
