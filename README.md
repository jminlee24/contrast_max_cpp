# Contrast Maximization C++

This project processes event-based camera data using contrast maximization optimization. The file reader is a modified version of the sample given in the Metavision SDK that decodes raw event data into arrays rather than CSV format.

## Prerequisites

The following libraries are required:

- **Eigen 3.4** - Matrix operations
- **OptimLib** (Header-only, Eigen-based) - Optimization library
- **nlohmann/json** - JSON parsing

## Installing Dependencies

### Option 1: System-wide Installation (Recommended)

#### Installing nlohmann/json

```bash
git clone https://github.com/nlohmann/json.git
cd json
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local -DJSON_BuildTests=OFF
cmake --build build --target install
```

#### Installing Eigen3

Eigen3 can typically be installed via your system's package manager:

**macOS (Homebrew):**
```bash
brew install eigen
```

**Linux (apt):**
```bash
sudo apt-get install libeigen3-dev
```

**Linux (yum):**
```bash
sudo yum install eigen3-devel
```

### Option 2: Local Installation

If you prefer to install dependencies locally:

1. Create a `packages` folder in the project root:
   ```bash
   mkdir packages
   ```

2. **For nlohmann/json:**
   ```bash
   cd packages
   git clone https://github.com/nlohmann/json.git
   cd json
   cmake -B build -DJSON_BuildTests=OFF
   cmake --build build
   cd ../..
   ```

3. **For Eigen:**
   ```bash
   cd packages
   # Download and extract Eigen 3.4.0
   wget https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
   tar -xzf eigen-3.4.0.tar.gz
   cd ..
   ```

4. When building, specify the package path:
   ```bash
   cmake -B build -DPACKAGE_PATH=$(pwd)/packages
   ```

## Building the Project

⚠️ **IMPORTANT: Always compile in Release mode!** Eigen will be approximately 1000x slower in Debug mode.

### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

The executable will be created at `build/main`.

### Using Local Packages

If you installed dependencies locally in a `packages` folder:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPACKAGE_PATH=../packages
cmake --build .
```

### Building on Windows

You can use the provided `build.bat` script, or manually:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Configuration

Before running the program, create a `config.json` file in the project root (you can copy `config.example.json` as a template):

```json
{
  "filepath": "../data/data.raw",
  "blur": true,
  "sliding_window": true,
  "timeslice": 1000,
  "generate_images": true
}
```

### Configuration Options

- **`filepath`**: Path to the raw event camera data file (.raw format)
- **`blur`**: Enable/disable Gaussian blur during optimization (`true`/`false`)
- **`sliding_window`**: Enable sliding window processing (`true`/`false`)
- **`timeslice`**: Time window size in microseconds for each optimization slice
- **`generate_images`**: Generate warped and previous images (`true`/`false`)

## Usage

1. **Prepare your configuration file:**
   ```bash
   cp config.example.json config.json
   # Edit config.json with your settings
   ```

2. **Run the program:**
   ```bash
   cd build
   ./main
   ```

   Or from the build directory:
   ```bash
   ./build/main
   ```

3. **Output:**
   - The program will display progress and optimization results
   - If `generate_images` is enabled, PGM images will be saved as `warped0.pgm`, `warped1.pgm`, etc., and `prev0.pgm`, `prev1.pgm`, etc.
   - Event data files (`normal.txt` and `warped.txt`) will also be generated

## Project Structure

- `src/main.cpp` - Main entry point
- `lib/` - Core libraries:
  - `filereader.cpp/hpp` - Event data file reading
  - `contrastmax.cpp/hpp` - Contrast maximization optimization
  - `optim/` - Optimization library (header-only)
- `data/` - Sample event camera data files
- `build/` - Build output directory (created during build)

## Notes

- The program processes event camera data by maximizing contrast through spatial warping
- Multiple optimization algorithms are available (with/without Gaussian blur)
- Processing can be done on the entire dataset or using sliding windows for temporal analysis
