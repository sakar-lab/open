# **open – A Simple Project Launcher & Config Manager for Linux/WSL**

`open` is a lightweight command-line tool written in C++ that lets you define multiple projects in a config file and launch them instantly using:

```bash
open <projectName>
```

It reads a structured config file (`~/.config/open/.config`) that contains a list of projects, each with:

* **Project name** (header)
* **IDE to launch** (e.g., `code`, `clion`, `qtcreator`)
* **Project path** (directory to open)

The tool then starts the requested program **in the background**, passing the project directory as its argument.

Works perfectly on **Linux** and **WSL**.

---

## ⭐ Features

* **Parse multiple projects** from a structured `.config` file
* **Launch an IDE or program in the background** using `fork + execvp`
* **Auto-expand `$HOME`** correctly to find the config file
* **Customizable setting types** (`ide`, `path`, etc.)
* **Fast block-based config parsing**
* Simple CLI:

  ```bash
  open <projectName>
  ```

---

## 📁 Config file format

Your config file lives at:

```
~/.config/open/.config
```

The format:

```
[projectName]
ide=code
path=/home/user/dev/MyProject

[anotherProject]
ide=clion
path=/home/user/dev/Another
```

Each block:

* Starts with `[projectName]`
* Followed by key=value pairs (`ide`, `path`, …)

---

## ▶️ Usage

### **List available projects**

If you run `open` with no arguments:

```bash
open
```

It prints usage help and lists all configured project names.

### **Launch a project**

```bash
open myproject
```

This:

* Reads the config entry `[myproject]`
* Gets the defined `ide` and `path`
* Spawns the IDE in the background:

Example (WSL):

```bash
code /home/user/dev/myproject &
```

Your terminal is **not blocked**, because the tool uses `fork()` and `execvp()` to start the process independently.

---

## 🔧 Installation

1. Build with CMake or g++
2. Place the binary somewhere in your `PATH`:

```bash
sudo mv open /usr/local/bin
```

(or symlink it)

```bash
ln -s /home/you/dev/open/build/open ~/.local/bin/open
```

Now you can use it anywhere.

---

## 🧠 Internals (how it works)

### **configManager**

* Loads the config file
* Splits it into logical blocks
* Extracts:

  * `projName`
  * All configured setting keys (`ide`, `path`, etc.)
* Stores them in:

  ```cpp
  std::pmr::map<std::string, std::string>
  ```

### **start_in_background**

A clean abstraction over:

* `fork()`
* `execvp()`

This launches the IDE without blocking the CLI.

---

## 📦 Example

Config:

```
[mazerunner]
ide=code
path=/home/alaska/dev/MazeRunnerOnline
```

Command:

```bash
open mazerunner
```

Result:

* VS Code opens the project directory
* Terminal stays free
* Full config printed for debugging

---

## 🛠 Planned Features (ideas)

* Auto-detect IDE paths via `which`
* Support more setting types
* Create/edit config entries from CLI
* Integration with CMake project scanning

