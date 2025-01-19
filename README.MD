# Instructions to Run the Python Application

## Prerequisites

Before running the application, ensure you have the following installed on your system:

1. Python 3.x
2. pip (Python package installer)
3. Make utility (optional, but recommended for ease of use)

## Steps to Run the Application

### 1. Clone or Download the Repository
Ensure you have the project directory that contains the following files:
- `main.py` (The main Python script)
- `Makefile` (For automating tasks)
- `requirements.txt` (List of dependencies)

### 2. Install Dependencies
Run the following command to install the required Python dependencies:

```bash
make install
```

This will:
- Read the `requirements.txt` file.
- Install the necessary Python packages, including Flask, OpenCV, and NumPy.

If `make` is not installed, you can manually run:

```bash
pip install -r requirements.txt
```

### 3. Run the Application
Execute the following command to run the application:

```bash
make run
```

This will:
- Install the dependencies (if not already installed).
- Execute the `main.py` script using Python.

If `make` is not installed, you can manually run:

```bash
python3 main.py
```

### 4. Optional Commands

#### Freeze Installed Dependencies
To update the `requirements.txt` file with the current installed dependencies:

```bash
make freeze
```

#### Clean Installed Dependencies
To uninstall all the dependencies listed in `requirements.txt`:

```bash
make clean
```

## Troubleshooting

- **Missing `make` utility**: Install it using your system’s package manager. For example:
  - On Debian/Ubuntu:
    ```bash
    sudo apt install make
    ```
  - On Fedora:
    ```bash
    sudo dnf install make
    ```

- **Permission issues**: Use `sudo` if needed (e.g., `sudo make install`).

- **Missing Python or pip**: Ensure Python 3.x and pip are installed and properly configured in your system’s PATH.

## Additional Notes

- Ensure you’re in the correct directory (where `main.py`, `Makefile`, and `requirements.txt` are located) before executing any commands.
- If you’re using a virtual environment, activate it before running the `make` or `pip` commands:

  ```bash
  source <venv_directory>/bin/activate
  ```


