# NVP - A Neovim Plugin Manager API (Alpha)

Hey everyone! I'm working on **NVP** (Neovim Plugin Manager), an open-source RESTful API-based package manager for Neovim plugins. The project is currently in **alpha** and I'm looking for contributors to help with development!

## What is NVP?

NVP is a centralized plugin management system for Neovim that allows you to:

- **Discover and manage** Neovim plugins through a clean API
- **Filter plugins** intelligently by:
  - Plugin name
  - Operating System (Linux, Windows, macOS)
  - Neovim distribution (NvChad, Astronvim, LazyVim, etc.)
- **Automate installations** using GitHub-hosted installation scripts
- **Store plugin metadata** including author, description, and repository URL

## How It Works

Each plugin entry includes a GitHub repository URL that contains an **installation shell script** (`install.sh`). When you find a plugin through NVP, you get all the metadata plus a direct link to the installation script for your specific OS/distro combination.

**Example Flow:**
1. Search for plugins: `GET /package/find/telescope/linux/nvchad`
2. Get back plugin details with GitHub URL
3. Run the installation script from the GitHub repo (customized for your setup)
4. Done! Plugin is installed.

## Tech Stack

- **Backend**: ASP.NET Core (.NET 10) with minimal APIs
- **Database**: RavenDB (document-based NoSQL)
- **Language**: C#

## Current Status: Alpha

The project is in early stages with basic CRUD operations working. Here's what's implemented:

✅ Store/retrieve plugins from RavenDB  
✅ Filter by name, OS, and distro  
✅ Basic API endpoints  
🔄 **Looking for help with:**
- Frontend UI/dashboard
- Plugin script validation & testing
- Authentication system
- Advanced search & filtering
- Installation orchestration
- Community contributions

## Why I'm Building This

Currently, Neovim plugins are scattered across GitHub with no unified discovery system. Beginners struggle to find quality plugins, and there's no standard way to manage installations across different setups. NVP aims to solve this!

## Get Involved

If you're interested in contributing, check out the [GitHub repository](link-to-repo). We're looking for:
- Backend developers (C#/.NET)
- Frontend developers (UI for plugin discovery)
- Plugin maintainers (to add your plugins to the registry)
- Testers & feedback

## Next Steps

- [ ] Build a web UI for plugin discovery
- [ ] Implement user authentication & API keys
- [ ] Add plugin rating/review system
- [ ] Create official plugin submission guidelines
- [ ] Build client libraries for easy integration
- [ ] Full Docker containerization

---

**TL;DR**: Making a centralized package manager for Neovim plugins with automatic OS/distro detection and shell script-based installations. In alpha, need help!

Feedback and contributions welcome! 🚀

---

**Questions? Want to contribute? Let me know in the comments!**

