# NVP - Neovim Plugin Manager

A RESTful API-based package manager and plugin manager for Neovim. NVP allows you to discover, manage, and install Neovim plugins with intelligent sorting by OS, Neovim distribution, and plugin name.

## Features

- **Package Management**: Store and retrieve Neovim plugin information
- **Smart Filtering**: Search and filter plugins by:
  - Plugin name
  - Operating System (OS)
  - Neovim distribution (NVD)
- **GitHub Integration**: Each plugin includes a GitHub URL pointing to an installation script
- **RESTful API**: Simple HTTP endpoints for all operations
- **RavenDB Integration**: Document-based NoSQL database for flexible data storage
- **Future Authentication**: Built with extensibility for authentication support

## Tech Stack

- **Framework**: ASP.NET Core (.NET 10)
- **Database**: RavenDB (Document Database)
- **Language**: C#

## Project Structure

```
NVP/
├── endPoints/              # API endpoint definitions
│   ├── AuthEndpoints.cs    # Authentication endpoints
│   └── PackageEndpoints.cs # Plugin/package endpoints
├── Models/                 # Data models
│   ├── package.cs          # Package/Plugin model
│   └── test.cs             # Test model
├── Services/               # Business logic
│   └── Tools.cs            # Utility functions
├── Infrastructure/         # Data access & infrastructure
│   └── RavenDbService.cs   # RavenDB service wrapper
├── Program.cs              # Application entry point
├── appsettings.json        # Configuration
└── compose.yaml            # Docker compose for services
```

## Installation & Setup

### Prerequisites

- .NET 10 SDK
- RavenDB instance (running on `http://192.168.1.20:8080`)

### Local Setup

1. **Clone the repository**
   ```bash
   cd NVP
   ```

2. **Restore NuGet packages**
   ```bash
   dotnet restore
   ```

3. **Configure RavenDB Connection**
   Update `Program.cs` with your RavenDB server URL and database name:
   ```csharp
   Urls = new[] { "http://192.168.1.20:8080" },
   Database = "NVP"
   ```

4. **Build the project**
   ```bash
   dotnet build
   ```

5. **Run the application**
   ```bash
   dotnet run
   ```

The API will be available at `http://localhost:5000` (or your configured port).

### Docker Setup

Use the provided `compose.yaml` to spin up RavenDB:
```bash
docker-compose up -d
```

## API Endpoints

### Test Endpoint
- **GET** `/package/test`
  - Returns a test package object
  - Example response:
    ```json
    {
      "id": "test",
      "RN": 123
    }
    ```

### Add/Save Plugin
- **POST** `/package/add`
  - Create and save a new plugin package
  - Request body:
    ```json
    {
      "id": "unique-id",
      "name": "plugin-name",
      "description": "Plugin description",
      "URL": "https://github.com/user/repo",
      "author": "author-name",
      "OS": "linux",
      "NVD": "neovim-distro"
    }
    ```

### Find Plugins
- **GET** `/package/find/{name}/{OS}/{NVD}`
  - Search for plugins by name, OS, and Neovim distribution
  - Example: `/package/find/telescope/linux/nvchad`
  - Returns a list of matching packages

## Data Model

### Package Structure
```csharp
public class package
{
    public string id { get; set; }           // Unique identifier
    public string name { get; set; }         // Plugin name
    public string description { get; set; }  // Plugin description
    public string URL { get; set; }          // GitHub repository URL
    public string author { get; set; }       // Plugin author
    public string OS { get; set; }           // Operating system (linux, windows, macos)
    public string NVD { get; set; }          // Neovim distribution
}
```

## Database

NVP uses **RavenDB**, a document-oriented NoSQL database. Each plugin is stored as a document, allowing flexible querying and filtering.

### Connection Details
- **Server**: http://192.168.1.20:8080
- **Database**: NVP
- **Type**: NoSQL Document Store

## Usage Example

### Adding a Plugin
```bash
curl -X POST http://localhost:5000/package/add \
  -H "Content-Type: application/json" \
  -d '{
    "id": "telescope",
    "name": "telescope.nvim",
    "description": "A highly extendable fuzzy finder",
    "URL": "https://github.com/nvim-telescope/telescope.nvim",
    "author": "nvim-telescope",
    "OS": "linux",
    "NVD": "nvchad"
  }'
```

### Finding Plugins
```bash
curl http://localhost:5000/package/find/telescope/linux/nvchad
```

## Future Enhancements

- 🔐 **Authentication**: User accounts and API keys
- 🔍 **Advanced Search**: Full-text search capabilities
- ⭐ **Ratings & Reviews**: Community feedback system
- 📊 **Analytics**: Download statistics and trending plugins
- 🚀 **Auto-Installation**: Direct plugin installation from the manager
- 📦 **Version Management**: Support for multiple plugin versions

## Configuration

Update `appsettings.json` for development/production settings:
```json
{
  "RavenDb": {
    "Urls": ["http://192.168.1.20:8080"],
    "Database": "NVP"
  }
}
```

## Development

The project uses ASP.NET Core's minimal API approach for lightweight and efficient endpoints. Services are registered in `Program.cs` using dependency injection.

## License

MIT License

## Contributing

[Contribution guidelines here]

