Here is a **clean, polished, professional-quality README rewrite** for your project.
Everything is reorganized, clearer, prettier, and easier for users to follow — while keeping all behaviors exactly as you described.

---

# 🚀 **open — A Simple Project Launcher & Config Manager**

`open` is a lightweight command-line tool for Linux/WSL that quickly launches your IDE or editor for a configured project:

```bash
open <projectName>
```

It reads a simple per-user configuration file and launches the program **in the background**, so your terminal remains free.

* **Language:** C++20
* **Build system:** CMake
* **Shell integration:** Bash completion + installer script
* **User config:** `~/.config/open/.config`

---

# ⭐ Quick Start

### 1. Install

From the project root:

```bash
chmod +x install.sh
./install.sh
```

The installer:

* Creates `~/.config/open/`
* Ensures `~/.config/open/.config` exists
* Installs the Bash completion script to `~/.config/open/auto_completion.sh`
* Adds a safe, idempotent snippet to your `~/.bashrc`
* Builds the project using CMake
* Installs the binary to `/usr/local/bin/open`
* Cleans the temporary `build/` directory

### 2. Reload your shell

```bash
source ~/.bashrc
```

### 3. Use it!

```bash
open                       # lists all configured projects
open myproject             # launches the IDE for that project
open -a newproj code .     # adds a project
open <proj><Tab>           # autocomplete
```

---

# 📁 Config File Format

User configuration file:

```
~/.config/open/.config
```

Example:

```
[mazerunner]
ide=code
path=/home/you/dev/MazeRunnerOnline

[anotherProject]
ide=clion
path=/home/you/dev/AnotherProject
```

Rules:

* Each project starts with `[projectName]`
* Followed by simple `key=value` pairs
* Supported keys:

  * `ide` — the program to launch
  * `path` — directory to open

A convenient shortcut:

```bash
open -a myproj code .
```

Here `.` expands to `$PWD`.

---

# 🧠 CLI Reference

### `open`

With no arguments:

* Prints usage
* Lists all configured project names

---

### `open <projectName>`

Launches the configured project:

* Reads the block `[projectName]`
* Extracts the `ide` and `path`
* Starts the IDE in the background using `fork()` + `execvp()`
* Prints the child PID

If the project isn’t found, prints an error.

---

### `open -a <projectName> <ide> <path>`

Appends a new project entry to your config file.

Examples:

```bash
open -a myapp code /home/you/dev/myapp
open -a testapp code .
```

---

### `open --commands`

Outputs items used by the autocompletion system:

```
-a project1 project2 project3 ...
```

Your Bash completion script relies on this output.

---

# ⚡ Bash Autocompletion

The repo includes an autocompletion script:

```
auto_completion.sh
```

The installer places it into:

```
~/.config/open/auto_completion.sh
```

and automatically adds this snippet to `~/.bashrc`:

```bash
if [ -f "$HOME/.config/open/auto_completion.sh" ]; then
    . "$HOME/.config/open/auto_completion.sh"
fi
```

After reload:

* Project names autocomplete:

  ```bash
  open ma<Tab>    # → mazerunner
  ```

* Flags autocomplete:

  ```bash
  open -<Tab>     # → -a
  ```

---

# 🛠 Building Manually (without install.sh)

```bash
mkdir -p build && cd build
cmake .. 
cmake --build . --target open --config Release
```

Binary appears in:

```
build/open
```

Install system-wide:

```bash
sudo cp open /usr/local/bin/open
sudo chmod +x /usr/local/bin/open
```

Or for user-local setup:

```bash
mkdir -p ~/.local/bin
cp open ~/.local/bin/open
chmod +x ~/.local/bin/open
```

Copy autocompletion (optional):

```bash
mkdir -p ~/.config/open
cp ../auto_completion.sh ~/.config/open/
```

Add sourcing to your `.bashrc`.

---

# 🔍 Code Overview

### `main.cpp`

* Argument parsing
* Project launching (`fork` + `execvp`)
* Adding new entries (`-a`)
* Providing project list (`--commands`)

### `configReader.*`

* Loads and parses `~/.config/open/.config`
* Stores settings in `std::pmr::map<std::string,std::string>`
* Provides:

  * `getConfigFor`
  * `getAllProjName`
  * `toString`

### `auto_completion.sh`

* Runs `open --commands`
* Uses Bash `compgen` to autocomplete project names and flags

### `install.sh`

* Complete automated installer
* Builds, installs, sets up completion, cleans temp build directory


---

# 💡 Example Workflow

```bash
cd ~/dev/myproject
open -a myproject code .

open myproject         # launch it later

open my<Tab>           # autocomplete
open                   # list all projects
```

